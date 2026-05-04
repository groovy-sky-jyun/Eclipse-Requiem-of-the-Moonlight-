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
	if (!BB || !Boss) return EBTNodeResult::Failed;

	EBossAttackType Attack = (EBossAttackType)BB->GetValueAsEnum(ABossAIController::BB_SelectedAttack);

	Boss->ExecuteAttack(Attack);

	float Duration = GetAttackDuration(Attack);

	TWeakObjectPtr<UBehaviorTreeComponent> WeakComp = &OwnerComp; //보스가 죽은 경우 대비

	GetWorld()->GetTimerManager().SetTimer(
		FinishTimer,
		[WeakComp, this]()
		{
			if (WeakComp.IsValid())
			{
				FinishLatentTask(*WeakComp.Get(), EBTNodeResult::Succeeded);
			}
		},
		Duration,
		false
	);

	// 해당 Task가 완료 신호를 보낼 때까지 대기
	return EBTNodeResult::InProgress;
}

void UBTTask_ExecuteAttack::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
	GetWorld()->GetTimerManager().ClearTimer(FinishTimer);
}

float UBTTask_ExecuteAttack::GetAttackDuration(EBossAttackType Attack) const
{
	// 실제 애님 몽타주 길이에 맞게 조정 필요 (Attack 지속 시간)
	switch (Attack)
	{
	case EBossAttackType::BloodBolt:	return 1.5f;
	case EBossAttackType::WraithDrop:	return 2.5f;
	case EBossAttackType::LunarBeam:	return 3.0f;
	case EBossAttackType::DamningTether:	return 4.0f;
	case EBossAttackType::MiasmaStep:	return 1.0f;
	case EBossAttackType::EclipseVeil:	return 8.0f; // 무적 지속 시간
	default: return 1.0f;
	}
}
