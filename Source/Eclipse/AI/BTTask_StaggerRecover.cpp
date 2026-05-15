// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_StaggerRecover.h"
#include "BossAIController.h"
#include "EnemyBoss.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_StaggerRecover::UBTTask_StaggerRecover()
{
	NodeName = TEXT("Stagger Recover");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_StaggerRecover::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AEnemyBoss* Boss = Cast<AEnemyBoss>(OwnerComp.GetAIOwner()->GetPawn());
	if (!BB || !Boss) return EBTNodeResult::Failed;

	ElapsedTime = 0.f;
	
	int32 Phase = BB->GetValueAsInt(ABossAIController::BB_CurrentPhase);
	StaggerDuration = (Phase == 1) ? 3.5f 
					: (Phase == 2) ? 2.8f 
					: 2.f;

	UE_LOG(LogTemp, Warning, TEXT("[Stagger] Start — Phase %d / Duration Time : %.1f 's"), Phase, StaggerDuration);

	return EBTNodeResult::InProgress;
}

void UBTTask_StaggerRecover::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	ElapsedTime += DeltaSeconds;

	if (ElapsedTime < StaggerDuration) return;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AEnemyBoss* Boss = Cast<AEnemyBoss>(OwnerComp.GetAIOwner()->GetPawn());

	if(!BB || !Boss) FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

	BB->SetValueAsBool(ABossAIController::BB_bIsStaggered, false);

	UE_LOG(LogTemp, Warning, TEXT("[Stagger] End"));
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}
