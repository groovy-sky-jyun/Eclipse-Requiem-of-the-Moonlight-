// Fill out your copyright notice in the Description page of Project Settings.


#include "BossPhaseComponent.h"
#include "Eclipse.h"
#include "EnemyBoss.h"
#include "BossAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBossPhaseComponent::UBossPhaseComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// 내림차순. 페이즈 1은 항상 HealthRatio 1.0에서 시작
	PhaseDataTable.Add({ 1.00f });
	PhaseDataTable.Add({ 0.70f });
	PhaseDataTable.Add({ 0.40f });
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
