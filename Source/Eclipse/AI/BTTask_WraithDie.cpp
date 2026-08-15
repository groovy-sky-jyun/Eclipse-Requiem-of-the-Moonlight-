// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_WraithDie.h"
#include "BaseEnemyAIController.h"
#include "CombatInterface.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_WraithDie::UBTTask_WraithDie()
{
	NodeName = TEXT("Wraith Die");
	bNotifyTick = false;
}

EBTNodeResult::Type UBTTask_WraithDie::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	APawn* Wraith = Cast<APawn>(AIController->GetPawn());
	if (!IsValid(Wraith) || !Wraith->Implements<UCombatInterface>()) return EBTNodeResult::Failed;

	// BT ÁßÁö
	OwnerComp.StopTree(EBTStopMode::Safe);

	ICombatInterface::Execute_Die(Wraith);

	return EBTNodeResult::Succeeded;
}

