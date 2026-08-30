// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_GroggyRecover.h"
#include "BossAIController.h"
#include "EnemyBoss.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_GroggyRecover::UBTTask_GroggyRecover()
{
	NodeName = TEXT("Groggy Recover");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_GroggyRecover::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AEnemyBoss* Boss = Cast<AEnemyBoss>(OwnerComp.GetAIOwner()->GetPawn());
	if (!BB || !Boss) return EBTNodeResult::Failed;

	ElapsedTime = 0.f;

	int32 Phase = BB->GetValueAsInt(ABossAIController::BB_CurrentPhase);
	GroggyDuration = (Phase == 1) ? 3.5f
					: (Phase == 2) ? 2.8f
					: 2.f;

	UE_LOG(LogTemp, Warning, TEXT("[Groggy] Start - Phase %d / Duration : %.1fs"), Phase, GroggyDuration);

	return EBTNodeResult::InProgress;
}

void UBTTask_GroggyRecover::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	ElapsedTime += DeltaSeconds;

	if (ElapsedTime < GroggyDuration) return;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AEnemyBoss* Boss = Cast<AEnemyBoss>(OwnerComp.GetAIOwner()->GetPawn());

	if (!BB || !Boss)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// 플래그를 내려야 데코레이터가 닫히고 전투로 돌아간다.
	BB->SetValueAsBool(ABossAIController::BB_bIsGroggy, false);

	UE_LOG(LogTemp, Warning, TEXT("[Groggy] End"));
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}
