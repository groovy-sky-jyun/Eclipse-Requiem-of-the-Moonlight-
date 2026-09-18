// Fill out your copyright notice in the Description page of Project Settings.


#include "BossGroggyComponent.h"
#include "Eclipse.h"
#include "EnemyBoss.h"
#include "BossAttackComponent.h"
#include "BossAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "TimerManager.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"

UBossGroggyComponent::UBossGroggyComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	PhaseSettings.Add({ 100.f, 4.f });
	PhaseSettings.Add({ 200.f, 4.f });
	PhaseSettings.Add({ 270.f, 4.f });
}

void UBossGroggyComponent::BeginPlay()
{
	Super::BeginPlay();

	Boss = Cast<AEnemyBoss>(GetOwner());
	if (!Boss)
	{
		UE_LOG(LogEclipse, Error, TEXT("[BossGroggy] Owner is not AEnemyBoss"));
		return;
	}
}

void UBossGroggyComponent::AddStagger(int32 CurrentPhase, float StaggerValue)
{
	// (1) 사망 상태 - 스태거 누적 x
	if (!IsBossAlive()) return;
	// (2)그로기 중, (3) 그로기 끝난 후 면역 상태 - 스태거 누적 x
	if (bIsGroggy || IsStaggerImmune()) return;

	if (!PhaseSettings.IsValidIndex(CurrentPhase - 1))
	{
		UE_LOG(LogEclipse, Error, TEXT("[BossGroggy] Phase %d not found in PhaseSettings (%d rows)"), CurrentPhase, PhaseSettings.Num());
		return;
	}

	// 맞는 동안은 줄지 않는다. 대기 시간을 처음부터 다시 잰다.
	StopStaggerDecay();

	const float MaxStagger = PhaseSettings[CurrentPhase - 1].StaggerThreshold;
	CurrentStagger = FMath::Min(CurrentStagger + StaggerValue, MaxStagger);

	if (CurrentStagger >= MaxStagger)
	{
		EnterGroggy();
		return;
	}

	if (StaggerDecayDelay <= 0.f)
	{
		StartStaggerDecay();
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(
		StaggerDecayDelayHandle,
		FTimerDelegate::CreateUObject(this, &UBossGroggyComponent::StartStaggerDecay),
		StaggerDecayDelay,
		false);
}

void UBossGroggyComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsGroggy || !IsBossAlive())
	{
		StopStaggerDecay();
		return;
	}

	CurrentStagger = FMath::Max(0.f, CurrentStagger - StaggerDecayPerSecond * DeltaTime);
	if (CurrentStagger <= 0.f)
	{
		StopStaggerDecay();
	}
}

void UBossGroggyComponent::StartStaggerDecay()
{
	if (bIsGroggy || CurrentStagger <= 0.f || StaggerDecayPerSecond <= 0.f) return;

	SetComponentTickEnabled(true);
}

void UBossGroggyComponent::StopStaggerDecay()
{
	SetComponentTickEnabled(false);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(StaggerDecayDelayHandle);
	}
}

void UBossGroggyComponent::SetGroggy(bool bNewGroggy)
{
	bIsGroggy = bNewGroggy;

	// BT가 그로기 중 공격을 고르지 않도록 알린다.
	if (IsValid(Boss) && Boss->BB)
	{
		Boss->BB->SetValueAsBool(ABossAIController::BB_bIsGroggy, bNewGroggy);
	}
}

bool UBossGroggyComponent::IsBossAlive() const
{
	return IsValid(Boss) && !ICombatInterface::Execute_IsDead(Boss);
}

bool UBossGroggyComponent::IsStaggerImmune() const
{
	const UWorld* World = GetWorld();
	return World && World->GetTimerManager().IsTimerActive(StaggerImmunityHandle);
}



// ── 그로기 ─────────────────────────────────────────────
void UBossGroggyComponent::EnterGroggy()
{
	// 이미 그로기 중이면 다시 진입하지 않는다.
	if (!IsBossAlive() || bIsGroggy) return;

	SetGroggy(true);
	StopStaggerDecay();
	UE_LOG(LogEclipse, Log, TEXT("[BossGroggy] Start!!!"));

	// 공격 단계와 관계없이 진행 중인 공격을 끊는다.
	if (UBossAttackComponent* AttackComp = Boss->GetAttackComponent())
	{
		AttackComp->CancelCurrent();
	}

	HoldGroggy();//후에 AnimNotify로 호출 로 변경
}

void UBossGroggyComponent::HoldGroggy()
{
	if (!bIsGroggy || !IsBossAlive()) return;

	UWorld* World = GetWorld();
	const int32 CurrentPhase = IsValid(Boss) ? Boss->GetCurrentPhase() : 1;

	// 유지 시간을 알 수 없으면 그로기에 갇히지 않도록 바로 기상한다.
	if (!World || !PhaseSettings.IsValidIndex(CurrentPhase - 1))
	{
		UE_LOG(LogEclipse, Error, TEXT("[BossGroggy] Phase %d not found in PhaseSettings (%d rows)"), CurrentPhase, PhaseSettings.Num());
		GetUp();
		return;
	}

	// SetTimer는 0초면 콜백 없이 해제되므로 직접 기상한다.
	const float Duration = PhaseSettings[CurrentPhase - 1].GroggyDuration;
	if (Duration <= 0.f)
	{
		GetUp();
		return;
	}

	World->GetTimerManager().SetTimer(
		GroggyHoldHandle,
		FTimerDelegate::CreateUObject(this, &UBossGroggyComponent::GetUp),
		Duration,
		false);
}

void UBossGroggyComponent::GetUp()
{
	// 그로기 도중 사망했으면 기상하지 않는다.
	if (!bIsGroggy || !IsBossAlive()) return;

	UAnimInstance* AnimInstance = (IsValid(Boss) && Boss->GetMesh()) ? Boss->GetMesh()->GetAnimInstance() : nullptr;
	if (!GetUpMontage || !AnimInstance || AnimInstance->Montage_Play(GetUpMontage) <= 0.f)
	{
		UE_LOG(LogEclipse, Warning, TEXT("[BossGroggy] GetUp montage not played"));
		FinishGroggy();
		return;
	}

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &UBossGroggyComponent::HandleGetUpMontageEnded);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, GetUpMontage);
}

void UBossGroggyComponent::HandleGetUpMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	FinishGroggy();
}

void UBossGroggyComponent::FinishGroggy()
{
	// 중복 호출 방지
	if (!bIsGroggy) return;

	SetGroggy(false);
	CurrentStagger = 0.f;
	StartStaggerImmunity();

	UE_LOG(LogEclipse, Log, TEXT("[BossGroggy] Groggy finished"));
}

void UBossGroggyComponent::StartStaggerImmunity()
{
	UWorld* World = GetWorld();
	if (!World || StaggerImmunityDuration <= 0.f) return;

	// 콜백 없이 건다. 만료 여부는 IsStaggerImmune()이 확인한다.
	World->GetTimerManager().SetTimer(StaggerImmunityHandle, StaggerImmunityDuration, false);
}
