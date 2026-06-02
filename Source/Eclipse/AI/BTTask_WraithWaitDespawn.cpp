// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_WraithWaitDespawn.h"
#include "WraithAIController.h"
#include "EnemyMinion.h"
#include "CombatInterface.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_WraithWaitDespawn::UBTTask_WraithWaitDespawn()
{
	NodeName = TEXT("Wraith Wait Despawn");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_WraithWaitDespawn::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ElapsedTime = 0.f;
	return EBTNodeResult::InProgress;
}

void UBTTask_WraithWaitDespawn::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	APawn* Wraith = Cast<APawn>(OwnerComp.GetAIOwner()->GetPawn());

	if (!BB || !Wraith) 
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	ElapsedTime += DeltaSeconds;

	if (ElapsedTime <= WaitDuration)
	{		
		if (Wraith->Implements<UCombatInterface>())
		{
			ICombatInterface::Execute_Die(Wraith);
		}

		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
}

EBTNodeResult::Type UBTTask_WraithWaitDespawn::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Aborted;
}
