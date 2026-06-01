// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_WraithMeleeAttack.h"
#include "WraithAIController.h"
#include "EnemyMinion.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CombatInterface.h"
#include "GameplayTagContainer.h"


UBTTask_WraithMeleeAttack::UBTTask_WraithMeleeAttack()
{
	NodeName = TEXT("Wraith Melee Attack");
	bNotifyTick = false;
}

EBTNodeResult::Type UBTTask_WraithMeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyMinion* Wraith = Cast<AEnemyMinion>(OwnerComp.GetAIOwner()->GetPawn());
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	APawn* Player = Cast<APawn>(BB->GetValueAsObject(AWraithAIController::BB_TargetActor));

	if (!Wraith || !BB || !Player) return EBTNodeResult::Failed;

	float Dist = FVector::Dist2D(Wraith->GetActorLocation(), Player->GetActorLocation());
	
	if (Dist > AttackRange) return EBTNodeResult::Failed;
	
	Wraith->Attack(); //타이머 둬야하는지?
	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UBTTask_WraithMeleeAttack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Aborted;
}
