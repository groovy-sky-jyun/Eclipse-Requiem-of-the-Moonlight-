// Fill out your copyright notice in the Description page of Project Settings.


#include "BossPhaseComponent.h"
#include "Eclipse.h"
#include "EnemyBoss.h"
#include "BossAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBossPhaseComponent::UBossPhaseComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
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
	if (!IsValid(Boss)) return 1;

	// 낮은 페이즈부터 보면 항상 1이 걸리므로 뒤에서부터 본다.
	for (int32 Phase = Boss->GetPhaseCount(); Phase >= 1; Phase--)
	{
		const FBossPhaseData* Settings = Boss->GetPhaseSettings(Phase);
		if (Settings && HealthRatio <= Settings->EnterHealthRatio)
		{
			return Phase;
		}
	}

	return 1;
}

void UBossPhaseComponent::EnterPhase(int32 NewPhase)
{
	if (CurrentPhase == NewPhase) return;

	if (!IsValid(Boss) || !Boss->GetPhaseSettings(NewPhase)) return;

	CurrentPhase = NewPhase;
	UE_LOG(LogEclipse, Log, TEXT("[BOSS] Enter : Phase %d"), CurrentPhase);

	if (Boss->BB)
	{
		Boss->BB->SetValueAsInt(ABossAIController::BB_CurrentPhase, CurrentPhase);
	}
}
