// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_WraithDie.h"
#include "WraithAIController.h"
#include "EnemyMinion.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_WraithDie::UBTTask_WraithDie()
{
	NodeName = TEXT("Wraith Die");
	bNotifyTick = false;
}

EBTNodeResult::Type UBTTask_WraithDie::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	APawn* Wraith = Cast<APawn>(OwnerComp.GetAIOwner()->GetPawn());

	if (!BB || !Wraith) return EBTNodeResult::Failed;
	
	if (Wraith->Implements<UCombatInterface>())
	{
		ICombatInterface::Execute_Die(Wraith);
	}

	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UBTTask_WraithDie::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Aborted;
}
