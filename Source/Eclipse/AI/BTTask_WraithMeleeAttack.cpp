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
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_WraithMeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyMinion* Wraith = Cast<AEnemyMinion>(OwnerComp.GetAIOwner()->GetPawn());
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	APawn* Player = Cast<APawn>(BB->GetValueAsObject(AWraithAIController::BB_TargetActor));

	if (!Wraith || !BB || !Player) return EBTNodeResult::Failed;

	/** 거리는 BB에 추가해서 실시간 UPDATE 하는 로직 넣기
	float Dist = FVector::Dist2D(Wraith->GetActorLocation(), Player->GetActorLocation());
	
	if (Dist > AttackRange) return EBTNodeResult::Failed;
	*/

	FTimerHandle TimerHandle;

	// Attack Finished 델리게이트 바인딩
	UBehaviorTreeComponent* OwnerCompPtr = &OwnerComp;
	Wraith->OnAttackFinishedDelegate.Clear();
	Wraith->OnAttackFinishedDelegate.AddLambda([this, OwnerCompPtr, Wraith, TimerHandle]() mutable
	{
			if (OwnerCompPtr && IsValid(Wraith))
			{
				Wraith->GetWorldTimerManager().ClearTimer(TimerHandle);
				FinishLatentTask(*OwnerCompPtr, EBTNodeResult::Succeeded);
			}
	});

	Wraith->AttackStart(); 

	// 3초 안에 액션 끝났다는 알림 안올 경우 예외 처리
	Wraith->GetWorldTimerManager().SetTimer(TimerHandle, [this, OwnerCompPtr, Wraith]()
		{
			if (OwnerCompPtr && IsValid(Wraith))
			{
				UE_LOG(LogTemp, Warning, TEXT("[Wraith MeleeAttack Task] not call attack finished."));
				Wraith->OnAttackFinishedDelegate.Clear();
				FinishLatentTask(*OwnerCompPtr, EBTNodeResult::Failed);
			}
		}, 3.f, false);

	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTTask_WraithMeleeAttack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Aborted;
}
