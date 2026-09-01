// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SelectAttack.h"
#include "Eclipse.h"
#include "BossAIController.h"
#include "EnemyBoss.h"
#include "BossAttackComponent.h"
#include "BossAttackBase.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SelectAttack::UBTTask_SelectAttack()
{
	NodeName = TEXT("Select Attack");
}

EBTNodeResult::Type UBTTask_SelectAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AAIController* AIController = OwnerComp.GetAIOwner();
	AEnemyBoss* Boss = AIController ? Cast<AEnemyBoss>(AIController->GetPawn()) : nullptr;
	if (!BB || !Boss) return EBTNodeResult::Failed;

	UBossAttackComponent* AttackComp = Boss->GetAttackComponent();
	if (!AttackComp) return EBTNodeResult::Failed;

	APawn* Target = Cast<APawn>(BB->GetValueAsObject(ABossAIController::BB_TargetActor));
	if (!Target) return EBTNodeResult::Failed;

	const int32 Phase = Boss->GetCurrentPhase();
	const TSubclassOf<UBossAttackBase> Selected = AttackComp->SelectAttack(Phase, Target);

	// 실패가 아니라 쓸 게 없는 것
	if (!Selected) return EBTNodeResult::Succeeded;

	UE_LOG(LogEclipse, Log, TEXT("[SelectAttack] Phase%d / Boss Attack : %s"), Phase, *Selected->GetName());

	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UBTTask_SelectAttack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Aborted;
}
