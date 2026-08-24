// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ExecuteAttack.h"
#include "BossAIController.h"
#include "EnemyBoss.h"
#include "BossAttackComponent.h"
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
	UBossAttackComponent* AttackComp = Boss ? Boss->GetAttackComponent() : nullptr;
	if (!BB || !Boss || !AttackComp)	return EBTNodeResult::Failed;

	EBossAttackType Attack = (EBossAttackType)BB->GetValueAsEnum(ABossAIController::BB_SelectedAttack);
	
	if (Attack == EBossAttackType::None)	return EBTNodeResult::Succeeded;
	
	AttackComp->OnAttackFinishedDelegate.Clear();

	TWeakObjectPtr<UBehaviorTreeComponent> WeakComp = &OwnerComp; //보스가 죽은 경우 대비

	AttackComp->OnAttackFinishedDelegate.AddLambda([this, WeakComp, AttackComp]()
		{
			if (WeakComp.IsValid())
			{
				AttackComp->OnAttackFinishedDelegate.Clear();
				FinishLatentTask(*WeakComp.Get(), EBTNodeResult::Succeeded);
			}
		}
	);

	AttackComp->ExecuteAttack(Attack);
	// 해당 Task가 완료 신호를 보낼 때까지 대기
	return EBTNodeResult::InProgress;
}

void UBTTask_ExecuteAttack::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return;

	if (AEnemyBoss* Boss = Cast<AEnemyBoss>(AIController->GetPawn()))
	{
		if (UBossAttackComponent* AttackComp = Boss->GetAttackComponent())
		{
			AttackComp->OnAttackFinishedDelegate.Clear();

			// Aborted는 공격이 아직 돌고 있다는 뜻이다. 델리게이트만 끊으면
			// 예약된 타이머는 그대로 남아, 스태거로 굳은 보스가 DarkSweep 경로를
			// 따라 계속 순간이동한다. 실행 중인 공격 자체를 끊어야 한다.
			if (TaskResult == EBTNodeResult::Aborted)
			{
				AttackComp->CancelCurrent();
			}
		}
	}
}


