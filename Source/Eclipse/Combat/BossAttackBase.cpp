// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAttackBase.h"
#include "Eclipse.h"
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
		UE_LOG(LogEclipse, Error, TEXT("[BossAttack] %s: Invalid owner"), *GetClass()->GetName());
		return;
	}

	if (IsRunning())
	{
		UE_LOG(LogEclipse, Warning, TEXT("[BossAttack] %s: Already running. Ignored"), *GetClass()->GetName());
		return;
	}

	Owner = InOwner;

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogEclipse, Error, TEXT("[BossAttack] %s: No world"), *GetClass()->GetName());
		Owner = nullptr;
		return;
	}

	SetAttackState(EBossAttackState::Windup);
	StartTime = World->GetTimeSeconds(); 

	// OnWindup()가 즉시 Finish()할 수 있다. ClearAllTimers 보다 먼저 만들어야 확실히 해제된다
	World->GetTimerManager().SetTimer(
		WatchdogHandle,
		FTimerDelegate::CreateUObject(this, &UBossAttackBase::OnWatchdogExpired),
		MaxDuration,
		false);

	// 후에 파생 클래스 몽타주 추가하면 AnimNotify로 변경 (지금은 임시)
	if (WindupTime > 0.f)
	{
		SetAttackTimer(
			PatternHandle,
			FTimerDelegate::CreateUObject(this, &UBossAttackBase::EnterActive),
			WindupTime,
			false);
	}

	UE_LOG(LogEclipse, Log, TEXT("[BossAttack] Start : %s"), *GetClass()->GetName());

	OnWindup();
}

void UBossAttackBase::EnterActive()
{
	if (!IsRunning())
	{
		return;
	}

	SetAttackState(EBossAttackState::Active);

	OnActive();
}

void UBossAttackBase::EnterRecovery()
{
	if (!IsRunning())
	{
		return;
	}

	SetAttackState(EBossAttackState::Recovery);

	if (RecoveryTime <= 0.f)
	{
		OnRecovery();
		Finish();
		return;
	}

	// OnRecovery()가 즉시 Finish할 수 있다. 타이머를 먼저 걸어야 확실히 해제된다.
	// RecoveryTime 값에 따라 Recovery Animation 길이가 정해진다.
	SetAttackTimer(
		PatternHandle,
		FTimerDelegate::CreateUObject(this, &UBossAttackBase::Finish),
		RecoveryTime,
		false);

	OnRecovery();
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

	const float Duration = GetElapsedTime();

	SetAttackState(EBossAttackState::Idle);

	ClearAllTimers();
	OnFinish();

	UE_LOG(LogEclipse, Log, TEXT("[BossAttack] Finish : %s (%.2fs)"), *GetClass()->GetName(), Duration);

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
	UE_LOG(LogEclipse, Warning,
		TEXT("[BossAttack] %s: Watchdog expired. Force finish"),
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
