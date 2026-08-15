// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BossAttack.h"
#include "Kismet/GameplayStatics.h"

const FName ABaseEnemyAIController::BB_TargetActor = TEXT("TargetActor");
const FName ABaseEnemyAIController::BB_bIsDead = TEXT("bIsDead");
const FName ABaseEnemyAIController::BB_DistanceToTarget = TEXT("DistanceToTarget");
const FName ABaseEnemyAIController::BB_bIsPlayerInRange = TEXT("bIsPlayerInRange");
const FName ABaseEnemyAIController::BB_CenterLocation = TEXT("CenterLocation");

ABaseEnemyAIController::ABaseEnemyAIController()
{
	BTComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BTComponent"));
	BlackboardComponent = CreateDefaultSubobject <UBlackboardComponent>(TEXT("BlackboardComponent"));
}

void ABaseEnemyAIController::OnPossess(APawn* InPawn)
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

	if (InPawn)
	{
		BlackboardComponent->SetValueAsVector(BB_CenterLocation, InPawn->GetActorLocation());
	}

	// 2. Set Blackboard Variable Default Value
	BlackboardComponent->SetValueAsFloat(BB_DistanceToTarget, 99999.f);
	BlackboardComponent->SetValueAsBool(BB_bIsPlayerInRange, false);
	
}

void ABaseEnemyAIController::OnUnPossess()
{
	Super::OnUnPossess();

	if (BTComponent)
	{
		BTComponent->StopTree(EBTStopMode::Safe);
	}
}
