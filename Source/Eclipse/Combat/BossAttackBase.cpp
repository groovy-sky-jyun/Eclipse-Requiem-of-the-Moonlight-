// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAttackBase.h"
#include "EnemyBoss.h"
#include "BossAttackComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

// ── 실행 제어 ────────────────────────────────────────────────
void UBossAttackBase::Begin(AEnemyBoss* InOwner)
{
	if (!IsValid(InOwner))
	{
		UE_LOG(LogTemp, Error, TEXT("[BossAttack] %s: Owner가 유효하지 않다."), *GetClass()->GetName());
		return;
	}

	if (IsRunning())
	{
		UE_LOG(LogTemp, Warning, TEXT("[BossAttack] %s: 이미 실행 중인데 다시 시작되었다. 무시한다."), *GetClass()->GetName());
		return;
	}

	Owner = InOwner;

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("[BossAttack] %s: World를 얻을 수 없다."), *GetClass()->GetName());
		Owner = nullptr;
		return;
	}

	SetAttackState(EBossAttackState::Windup);
	StartTime = World->GetTimeSeconds(); //종료 통보 누락 방지 워치독 타이머

	// OnStart()가 즉시 Finish()할 수 있다. ClearAllTimers 보다 먼저 만들어야 확실히 해제된다
	World->GetTimerManager().SetTimer(
		WatchdogHandle,
		FTimerDelegate::CreateUObject(this, &UBossAttackBase::OnWatchdogExpired),
		MaxDuration,
		false);

	OnStart();
}

void UBossAttackBase::Tick(float DeltaTime)
{
	if (!IsRunning())
	{
		return;
	}

	// 실행 도중 보스가 사라지면 더 진행할 의미가 없다.
	if (!IsValid(Owner))
	{
		Cancel();
		return;
	}

	OnTick(DeltaTime);
}

void UBossAttackBase::Cancel()
{
	if (!IsRunning())
	{
		return;
	}

	OnCancel();
	Finish();
}

void UBossAttackBase::Finish()
{
	if (!IsRunning())
	{
		return;
	}
	SetAttackState(EBossAttackState::Idle);

	ClearAllTimers();
	OnFinish();

	if (IsValid(Owner))
	{
		if (UBossAttackComponent* AttackComp = Owner->GetAttackComponent())
		{
			AttackComp->NotifyAttackFinished();
		}
	}
}

void UBossAttackBase::SetAttackState(EBossAttackState NewState)
{
	if (AttackState == NewState) return;
	AttackState = NewState;

	if (!IsValid(Owner)) return;
	if (UBossAttackComponent* AttackComp = Owner->GetAttackComponent())
	{
		AttackComp->NotifyAttackStateChanged(NewState);
	}
}

// ── 헬퍼 ────────────────────────────────────────────────────
UWorld* UBossAttackBase::GetWorld() const
{
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		return nullptr;
	}

	if (IsValid(Owner))
	{
		return Owner->GetWorld();
	}

	return GetOuter() ? GetOuter()->GetWorld() : nullptr;
}

APawn* UBossAttackBase::GetTargetPlayer() const
{
	UWorld* World = GetWorld();
	return World ? UGameplayStatics::GetPlayerPawn(World, 0) : nullptr;
}

float UBossAttackBase::GetElapsedTime() const
{
	UWorld* World = GetWorld();
	return World ? (World->GetTimeSeconds() - StartTime) : 0.f;
}

void UBossAttackBase::SetAttackTimer(FTimerHandle& OutHandle, FTimerDelegate Delegate, float Delay, bool bLoop)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	World->GetTimerManager().SetTimer(OutHandle, Delegate, Delay, bLoop);
	ActiveTimers.Add(OutHandle);
}

void UBossAttackBase::ClearAttackTimer(FTimerHandle& Handle)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(Handle);
	}
	ActiveTimers.Remove(Handle);
}

// ── 내부 ────────────────────────────────────────────────────
void UBossAttackBase::OnWatchdogExpired()
{
	UE_LOG(LogTemp, Error,
		TEXT("[BossAttack] %s: 워치독 발동. Attack 강제 종료"),
		*GetClass()->GetName());

	Cancel();
}

void UBossAttackBase::ClearAllTimers()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		ActiveTimers.Reset();
		return;
	}

	FTimerManager& TimerManager = World->GetTimerManager();
	TimerManager.ClearTimer(WatchdogHandle);

	for (FTimerHandle& Handle : ActiveTimers)
	{
		TimerManager.ClearTimer(Handle);
	}
	ActiveTimers.Reset();
}
