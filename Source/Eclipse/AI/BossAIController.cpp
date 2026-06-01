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

	RunBehaviorTree(BehaviorTreeAsset);
}

void ABossAIController::OnUnPossess()
{
	Super::OnUnPossess();
}
