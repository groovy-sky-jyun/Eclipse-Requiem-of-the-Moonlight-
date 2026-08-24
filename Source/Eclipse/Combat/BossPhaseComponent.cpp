// Fill out your copyright notice in the Description page of Project Settings.


#include "BossPhaseComponent.h"
#include "EnemyBoss.h"
#include "BossAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UBossPhaseComponent::UBossPhaseComponent()
{
	// 페이즈 판정은 BTService_UpdatePhase가 주기적으로 물어본다.
	PrimaryComponentTick.bCanEverTick = false;
}

void UBossPhaseComponent::BeginPlay()
{
	Super::BeginPlay();

	Boss = Cast<AEnemyBoss>(GetOwner());

	// 페이즈 1은 EnterPhase를 거치지 않는다(CurrentPhase가 이미 1이라 조기 반환).
	// 시작 임계값은 여기서 직접 맞춰준다.
	UpdateStaggerThresholdByPhase();
	if (!Boss)
	{
		UE_LOG(LogTemp, Error, TEXT("[BossPhase] Owner가 AEnemyBoss가 아니다."));
	}
}


// ── 페이즈 ─────────────────────────────────────────────
void UBossPhaseComponent::EnterPhase(int32 NewPhase)
{
	if (CurrentPhase == NewPhase) return;

	CurrentPhase = NewPhase;
	UE_LOG(LogTemp, Warning, TEXT("[BOSS] Enter : Phase %d"), CurrentPhase);

	UpdateStaggerThresholdByPhase();

	if (!IsValid(Boss)) return;

	switch (CurrentPhase)
	{
	case 2:
		Boss->bEclipseVeilUsed = false;
		break;

	case 3:
		Boss->GetCharacterMovement()->MaxFlySpeed = 900.f;
		break;

	default:
		break;
	}
}


// ── Stagger ──────────────────────────────────────────────────────
void UBossPhaseComponent::NotifyDamageTaken(float DamageAmount)
{
	if (!IsValid(Boss) || !Boss->AI || !Boss->BB) return;

	// 이미 스태거 중이면 누적하지 않는다.
	// 회복 도중에 임계값을 또 넘기면 BB가 다시 true로 덮여 BTTask_StaggerRecover가
	// 처음부터 다시 시작되고, 맞는 동안 보스가 영영 못 일어난다.
	if (Boss->BB->GetValueAsBool(ABossAIController::BB_bIsStaggered)) return;

	float Now = GetWorld()->GetTimeSeconds();
	if ((Now - TimeSinceLastHit) > StaggerResetTime) //StaggerResetTime 안에 다음 피격이 들어와야함.
	{
		StaggerAccumulated = 0.f;
	}
	TimeSinceLastHit = Now;

	StaggerAccumulated += DamageAmount;

	if (StaggerAccumulated >= StaggerThreshold)
	{
		// 스태거 발동
		StaggerAccumulated = 0.f;
		Boss->BB->SetValueAsBool(ABossAIController::BB_bIsStaggered, true);
	}
}

void UBossPhaseComponent::UpdateStaggerThresholdByPhase()
{
	if (StaggerThresholdByPhase.Num() == 0) return;

	const int32 Index = FMath::Clamp(CurrentPhase - 1, 0, StaggerThresholdByPhase.Num() - 1);
	StaggerThreshold = StaggerThresholdByPhase[Index];
}
