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

	UE_LOG(LogTemp, Warning, TEXT("[WraithWaitDespawn] Destroy"));
	Wraith->Destroy();
	// 보스에게 망령 소멸 알려서 Blackboard 의 남아있는 망령수 줄여야함.

	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UBTTask_WraithDie::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Aborted;
}
