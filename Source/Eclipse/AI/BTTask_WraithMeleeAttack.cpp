// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_WraithMeleeAttack.h"
#include "Eclipse.h"
#include "WraithAIController.h"
#include "EnemyMinion.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTTask_WraithMeleeAttack::UBTTask_WraithMeleeAttack()
{
	NodeName = TEXT("Wraith Melee Attack");
	bNotifyTick = false;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_WraithMeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyMinion* Wraith = Cast<AEnemyMinion>(OwnerComp.GetAIOwner()->GetPawn());
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	APawn* Player = Cast<APawn>(BB->GetValueAsObject(AWraithAIController::BB_TargetActor));

	if (!Wraith || !BB || !Player) return EBTNodeResult::Failed;

	//타이머 작동 중 죽을 경우 대비하여 약한 참조 포인터 사용
	TWeakObjectPtr<UBehaviorTreeComponent> OwnerCompPtr(&OwnerComp);

	// 3초 안에 액션 끝났다는 알림 안올 경우 예외 처리
	Wraith->GetWorldTimerManager().SetTimer(AttackTimerHandle, [this, OwnerCompPtr, Wraith]()
		{
			if (OwnerCompPtr.IsValid() && IsValid(Wraith))
			{
				UE_LOG(LogEclipse, Warning, TEXT("[Wraith MeleeAttack Task] not call attack finished."));
				Wraith->OnAttackFinishedDelegate.Clear();
				FinishLatentTask(*OwnerCompPtr, EBTNodeResult::Failed);
			}
		}, 3.f, false);

	// Attack Finished 델리게이트 바인딩
	Wraith->OnAttackFinishedDelegate.Clear();
	Wraith->OnAttackFinishedDelegate.AddLambda([this, OwnerCompPtr, Wraith]()
	{
			if (OwnerCompPtr.IsValid() && IsValid(Wraith))
			{
				Wraith->GetWorldTimerManager().ClearTimer(AttackTimerHandle);
				FinishLatentTask(*OwnerCompPtr, EBTNodeResult::Succeeded);
			}
	});

	Wraith->AttackStart(); 

	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTTask_WraithMeleeAttack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyMinion* Wraith = Cast<AEnemyMinion>(OwnerComp.GetAIOwner()->GetPawn());

	if (Wraith)
	{
		Wraith->GetWorldTimerManager().ClearTimer(AttackTimerHandle);
		Wraith->OnAttackFinishedDelegate.Clear();
	}

	return EBTNodeResult::Aborted;
}
