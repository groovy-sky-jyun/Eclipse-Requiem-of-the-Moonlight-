// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_ReturnToHome.h"
#include "BossAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ReturnToHome::UBTTask_ReturnToHome()
{
	NodeName = TEXT("ReturnToHome");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_ReturnToHome::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;

	WaitRemaining = WaitBeforeReturn;
	bWaiting = true;

	return EBTNodeResult::InProgress;
}

void UBTTask_ReturnToHome::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	APawn* Boss = OwnerComp.GetAIOwner()->GetPawn();
	if (!BB || !Boss) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); }

	// 대기 중 플레이어 감지 시 귀환 중단 -> 전투 재개
	if (BB->GetValueAsBool(ABossAIController::BB_bIsPlayerInRange))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	// 3초 대기
	if (bWaiting)
	{
		WaitRemaining -= DeltaSeconds;
		if (WaitRemaining > 0.f) return;
		bWaiting = false;
	}

	// 귀환
	FVector HomeLocation = BB->GetValueAsVector(ABossAIController::BB_CenterLocation);
	FVector Current = Boss->GetActorLocation();
	float Dist = FVector::Dist(Current, HomeLocation);

	if (Dist <= AcceptanceRadius)
	{
		BB->SetValueAsBool(ABossAIController::BB_bIsInCombat, false);
		BB->SetValueAsFloat(ABossAIController::BB_OrbitAngle, 0);

		UE_LOG(LogTemp, Warning, TEXT("[ReturnToHome] Return Success ? Idle"));

		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	FVector Dir = (HomeLocation - Current).GetSafeNormal();
	Boss->SetActorLocation(Current + Dir * ReturnSpeed * DeltaSeconds);
}

EBTNodeResult::Type UBTTask_ReturnToHome::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Aborted;
}
