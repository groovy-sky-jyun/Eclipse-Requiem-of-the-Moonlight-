// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_WraithWaitDespawn.h"
#include "WraithAIController.h"
#include "EnemyMinion.h"
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

	/* 플레이어가 다시 경기장 안으로 들어오면 즉시 중단 → 전투 재개
	if (BB->GetValueAsBool(AWraithAIController::BB_bIsPlayerInRange))
	{
		UE_LOG(LogTemp, Warning, TEXT("[WraithWaitDespawn] 플레이어 복귀 → 전투 재개"));
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}*/ 
	// 어차피 데코레이션에서 걸러지니가 이 코드는 필요없지 않나? 

	ElapsedTime += DeltaSeconds;

	if (ElapsedTime <= WaitDuration)
	{
		UE_LOG(LogTemp, Warning, TEXT("[WraithWaitDespawn] Destroy"));
		Wraith->Destroy();
		// 보스에게 망령 소멸 알려서 Blackboard 의 남아있는 망령수 줄여야함.
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
}

EBTNodeResult::Type UBTTask_WraithWaitDespawn::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Aborted;
}
