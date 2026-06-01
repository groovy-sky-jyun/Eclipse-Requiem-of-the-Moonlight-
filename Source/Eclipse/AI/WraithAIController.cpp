// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/WraithAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BossAttack.h"
#include "Kismet/GameplayStatics.h"

const FName AWraithAIController::BB_bIsDead = TEXT("bIsDead");

AWraithAIController::AWraithAIController()
{
}

void AWraithAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!BehaviorTreeAsset || !BlackboardComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("BossAIController: BehaviorTreeAsset is NULL"));
		return;
	}


	BlackboardComponent->SetValueAsBool(BB_bIsDead, true);

	RunBehaviorTree(BehaviorTreeAsset);
}

void AWraithAIController::OnUnPossess()
{
	Super::OnUnPossess();
}
