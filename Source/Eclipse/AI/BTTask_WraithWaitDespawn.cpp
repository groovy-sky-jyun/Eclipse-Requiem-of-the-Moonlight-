// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_WraithWaitDespawn.h"
#include "AIController.h"
#include "EnemyMinion.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_WraithWaitDespawn::UBTTask_WraithWaitDespawn()
{
	NodeName = TEXT("Wraith Wait DeSpawn");
	bNotifyTick = false;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_WraithWaitDespawn::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AEnemyMinion* Wraith = Cast<AEnemyMinion>(AIController->GetPawn());
	if (!BB || !IsValid(Wraith)) return EBTNodeResult::Failed;

	AIController->StopMovement();

	//타이머 작동 중 죽을 경우 대비하여 약한 참조 포인터 사용
	TWeakObjectPtr<UBehaviorTreeComponent> OwnerCompWeakPtr(&OwnerComp);
	if (!OwnerCompWeakPtr.IsValid()) return EBTNodeResult::Failed;

	Wraith->GetWorldTimerManager().SetTimer(DespawnTimerHandle, [Wraith, OwnerCompWeakPtr]()
		{
			OwnerCompWeakPtr->StopTree(EBTStopMode::Safe);
			ICombatInterface::Execute_Die(Wraith);
			return EBTNodeResult::Succeeded;
		}, 4.f, false);

	return EBTNodeResult::InProgress;
	
}

EBTNodeResult::Type UBTTask_WraithWaitDespawn::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyMinion* Wraith = Cast<AEnemyMinion>(OwnerComp.GetAIOwner()->GetPawn());
	if (Wraith)
	{
		Wraith->GetWorldTimerManager().ClearTimer(DespawnTimerHandle);
	}

	return EBTNodeResult::Aborted;
}

