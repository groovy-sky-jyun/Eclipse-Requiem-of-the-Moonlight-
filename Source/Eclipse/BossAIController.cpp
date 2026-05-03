// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BossAttack.h"

const FName ABossAIController::BB_TargetActor = TEXT("TargetActor");
const FName ABossAIController::BB_CurrentPhase = TEXT("CurrentPhase");
const FName ABossAIController::BB_IsInCombat = TEXT("IsInCombat");
const FName ABossAIController::BB_DistanceToTarget = TEXT("DistanceToTarget");
const FName ABossAIController::BB_SelectedAttack = TEXT("SelectedAttack");
const FName ABossAIController::BB_CanReceiveDamage = TEXT("CanReceiveDamage");
const FName ABossAIController::BB_ActiveWraithCount = TEXT("ActiveWraithCount");
const FName ABossAIController::BB_UsedUltimateAttack = TEXT("UsedUltimateAttack");

ABossAIController::ABossAIController()
{
	BTComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BTComponent"));
	BlackboardComponent = CreateDefaultSubobject <UBlackboardComponent>(TEXT("BlackboardComponent"));
}

void ABossAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!BehaviorTreeAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("BossAIController: BehaviorTreeAsset is NULL"));
		return;
	}

	// 1. Init Blackboard
	if (UBlackboardData* BBData = BehaviorTreeAsset->BlackboardAsset)
	{
		UseBlackboard(BBData, BlackboardComponent);
	}

	// 2. Set Blackboard Variable Default Value
	BlackboardComponent->SetValueAsInt(BB_CurrentPhase, 1);
	BlackboardComponent->SetValueAsBool(BB_IsInCombat, false);
	BlackboardComponent->SetValueAsFloat(BB_DistanceToTarget, 99999.f);
	BlackboardComponent->SetValueAsEnum(BB_SelectedAttack, static_cast<uint8>(EBossAttackType::None));
	BlackboardComponent->SetValueAsBool(BB_CanReceiveDamage, true);
	BlackboardComponent->SetValueAsInt(BB_ActiveWraithCount, 0);
	BlackboardComponent->SetValueAsBool(BB_UsedUltimateAttack, false);

	RunBehaviorTree(BehaviorTreeAsset);
}

void ABossAIController::OnUnPossess()
{
	Super::OnUnPossess();

	if (BTComponent)
	{
		BTComponent->StopTree(EBTStopMode::Safe);
	}
}
