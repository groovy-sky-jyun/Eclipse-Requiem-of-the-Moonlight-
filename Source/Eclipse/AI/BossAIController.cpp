// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BossAttack.h"
#include "Kismet/GameplayStatics.h"


const FName ABossAIController::BB_SelectedAttack = TEXT("SelectedAttack");
const FName ABossAIController::BB_bCanReceiveDamage = TEXT("bCanReceiveDamage");
const FName ABossAIController::BB_ActiveWraithCount = TEXT("ActiveWraithCount");
const FName ABossAIController::BB_OrbitAngle = TEXT("OrbitAngle");
const FName ABossAIController::BB_bIsStaggered = TEXT("bIsStaggered");
const FName ABossAIController::BB_CurrentPhase = TEXT("CurrentPhase");
const FName ABossAIController::BB_bIsInCombat = TEXT("bIsInCombat");


ABossAIController::ABossAIController()
{

}

void ABossAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!BehaviorTreeAsset|| !BlackboardComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("BossAIController: BehaviorTreeAsset is NULL"));
		return;
	}

	BlackboardComponent->SetValueAsEnum(BB_SelectedAttack, static_cast<uint8>(EBossAttackType::None));
	BlackboardComponent->SetValueAsBool(BB_bCanReceiveDamage, true);
	BlackboardComponent->SetValueAsInt(BB_ActiveWraithCount, 0);
	BlackboardComponent->SetValueAsFloat(BB_OrbitAngle, 0.f);
	BlackboardComponent->SetValueAsBool(BB_bIsStaggered, false);
	BlackboardComponent->SetValueAsInt(BB_CurrentPhase, 1);
	BlackboardComponent->SetValueAsBool(BB_bIsInCombat, false);
	BlackboardComponent->SetValueAsBool(BB_bIsDead, false);

	RunBehaviorTree(BehaviorTreeAsset);
}

void ABossAIController::OnUnPossess()
{
	Super::OnUnPossess();
}
