// Fill out your copyright notice in the Description page of Project Settings.


#include "BossPhaseComponent.h"
#include "Eclipse.h"
#include "EnemyBoss.h"
#include "BossAIController.h"
#include "BossAttackComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UBossPhaseComponent::UBossPhaseComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// 내림차순. 페이즈 1은 항상 HealthRatio 1.0에서 시작
	PhaseDataTable.Add({ 1.00f, 100.f });
	PhaseDataTable.Add({ 0.70f, 200.f });
	PhaseDataTable.Add({ 0.40f, 270.f });
}

void UBossPhaseComponent::BeginPlay()
{
	Super::BeginPlay();

	Boss = Cast<AEnemyBoss>(GetOwner());
	if (!Boss)
	{
		UE_LOG(LogEclipse, Error, TEXT("[BossPhase] Owner is not AEnemyBoss"));
		return;
	}

	// ABaseCharacter::BeginPlay의 SetHealth(MaxHealth)가 실행되므로, 초기 브로드캐스트도 여기서 받는다.
	Boss->OnHealthChangedDelegate.AddDynamic(this, &UBossPhaseComponent::HandleHealthChanged);

	if (UBossAttackComponent* AttackComp = Boss->GetAttackComponent())
	{
		AttackComp->OnAttackStateChangedDelegate.AddUObject(this, &UBossPhaseComponent::HandleAttackStateChanged);
	}
}

const FBossPhaseData& UBossPhaseComponent::GetCurrentPhaseData() const
{
	// 배열이 비어도 호출부가 null 검사를 하지 않아도 되도록 기본값 return;
	static const FBossPhaseData Fallback;

	return PhaseDataTable.IsValidIndex(CurrentPhase - 1) ? PhaseDataTable[CurrentPhase - 1] : Fallback;
}


// ── 페이즈 ─────────────────────────────────────────────
void UBossPhaseComponent::HandleHealthChanged(float Current, float Max)
{
	if (Max <= 0.f) return;

	const float HealthRatio = Current / Max;
	if (HealthRatio <= 0.f) return;

	const int32 NewPhase = FindPhase(HealthRatio);
	if (NewPhase <= CurrentPhase) return;

	EnterPhase(NewPhase);
}

int32 UBossPhaseComponent::FindPhase(float HealthRatio) const
{
	for (int32 Index = PhaseDataTable.Num() - 1; Index >= 0; Index--)
	{
		if (HealthRatio <= PhaseDataTable[Index].EnterHealthRatio)
		{
			return Index + 1;
		}
	}

	return 1;
}

void UBossPhaseComponent::EnterPhase(int32 NewPhase)
{
	if (CurrentPhase == NewPhase) return;

	if (!PhaseDataTable.IsValidIndex(NewPhase - 1))
	{
		UE_LOG(LogEclipse, Error, TEXT("[BossPhase] Phase %d not found in PhaseDataTable (%d rows)"), NewPhase, PhaseDataTable.Num());
		return;
	}

	CurrentPhase = NewPhase;
	UE_LOG(LogEclipse, Log, TEXT("[BOSS] Enter : Phase %d"), CurrentPhase);

	if (!IsValid(Boss)) return;
	if (Boss->BB)
	{
		Boss->BB->SetValueAsInt(ABossAIController::BB_CurrentPhase, CurrentPhase);
	}
}


// ── Stagger ──────────────────────────────────────────────────────
void UBossPhaseComponent::AddStaggerDamage(float DamageAmount)
{
	if (!IsValid(Boss) || !Boss->AI || !Boss->BB) return;

	// 그로기 중에 맞은 피해는 게이지에 쌓지 않는다.
	if (Boss->BB->GetValueAsBool(ABossAIController::BB_bIsGroggy)) return;

	// 1) 마지막 피격으로 부터 StaggerResetTime 안에 피격이 다시 안오면 쌓인 게이지 버림
	float Now = GetWorld()->GetTimeSeconds();
	if ((Now - TimeSinceLastHit) > StaggerResetTime) 
	{
		StaggerAccumulated = 0.f;
	}
	TimeSinceLastHit = Now;

	StaggerAccumulated += DamageAmount;

	// 2) 현재 페이즈의 그로기 임계값 넘었는지 확인
	if (StaggerAccumulated >= GetCurrentPhaseData().StaggerThreshold)
	{
		StaggerAccumulated = 0.f;

		UBossAttackComponent* AttackComp = Boss->GetAttackComponent();
		const EBossAttackState State = AttackComp ? AttackComp->GetAttackState() : EBossAttackState::Idle;

		// 3) Startup / Active 중에는 공격을 끊지 않고 Recovery까지 기다린다.
		if (State == EBossAttackState::Startup || State == EBossAttackState::Active)
		{
			bGroggyPending = true;
			return;
		}

		TriggerGroggy();
	}
}

void UBossPhaseComponent::TriggerGroggy()
{
	if (!IsValid(Boss) || !Boss->BB) return;

	bGroggyPending = false;
	Boss->BB->SetValueAsBool(ABossAIController::BB_bIsGroggy, true);

	UE_LOG(LogEclipse, Log, TEXT("[BossPhase] Stagger filled!!!"));
}

void UBossPhaseComponent::HandleAttackStateChanged(EBossAttackState NewState)
{
	if (!bGroggyPending) return;

	if (NewState != EBossAttackState::Recovery && NewState != EBossAttackState::Idle) return;

	TriggerGroggy();
}
