// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ExecuteAttack.h"
#include "BossAIController.h"
#include "EnemyBoss.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ExecuteAttack::UBTTask_ExecuteAttack()
{
	NodeName = TEXT("Execute Attack");
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_ExecuteAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AEnemyBoss* Boss = Cast<AEnemyBoss>(OwnerComp.GetAIOwner()->GetPawn());
	if (!BB || !Boss)	return EBTNodeResult::Failed;

	EBossAttackType Attack = (EBossAttackType)BB->GetValueAsEnum(ABossAIController::BB_SelectedAttack);
	
	if (Attack == EBossAttackType::None)	return EBTNodeResult::Succeeded;
	
	Boss->OnAttackFinishedDelegate.Clear();

	TWeakObjectPtr<UBehaviorTreeComponent> WeakComp = &OwnerComp; //보스가 죽은 경우 대비

	Boss->OnAttackFinishedDelegate.AddLambda([this, WeakComp, Boss]()
		{
			if (WeakComp.IsValid())
			{
				Boss->OnAttackFinishedDelegate.Clear();
				FinishLatentTask(*WeakComp.Get(), EBTNodeResult::Succeeded);
			}
		}
	);

	Boss->ExecuteAttack(Attack);
	// 해당 Task가 완료 신호를 보낼 때까지 대기
	return EBTNodeResult::InProgress;
}

void UBTTask_ExecuteAttack::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	if (AEnemyBoss* Boss = Cast<AEnemyBoss>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		Boss->OnAttackFinishedDelegate.Clear();
	}
}


