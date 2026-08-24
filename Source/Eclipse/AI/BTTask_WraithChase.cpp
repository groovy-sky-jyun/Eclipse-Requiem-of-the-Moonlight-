// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_WraithChase.h"
#include "WraithAIController.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "PlayerCharacter.h"

UBTTask_WraithChase::UBTTask_WraithChase()
{
	NodeName = TEXT("Wraith Chase");
	bNotifyTick = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_WraithChase::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();

	// BB를 쓰기 전에 검사해야 한다. 아래 GetValueAsObject가 먼저 오면 널 역참조다.
	if (!BB || !AIController) return EBTNodeResult::Failed;

	APawn* Player = Cast<APawn>(BB->GetValueAsObject(AWraithAIController::BB_TargetActor));
	if (!IsValid(Player)) return EBTNodeResult::Failed;

	// 이 노드는 재진입한다. 이전 실행의 누적치를 반드시 지운다.
	TaskElapsedTime = 0.f;
	
	CachedPlayerLoc = Player->GetActorLocation();
	FVector TargetLoc = BB->GetValueAsVector(AWraithAIController::BB_TargetLocation);
	EPathFollowingRequestResult::Type MoveResult = AIController->MoveToLocation(TargetLoc, AcceptanceRadius);

	if (MoveResult == EPathFollowingRequestResult::Failed)
	{
		return EBTNodeResult::Failed;
	}
	else if (MoveResult == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::InProgress;
}

void UBTTask_WraithChase::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	APawn* Player = Cast<APawn>(BB->GetValueAsObject(AWraithAIController::BB_TargetActor));

	if (!BB || !IsValid(Player) || !AIController)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// 목적지에 도착했는지 확인
	if (AIController->GetMoveStatus() == EPathFollowingStatus::Idle)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	TaskElapsedTime += DeltaSeconds;

	float DistanceMoved = FVector::Dist2D(Player->GetActorLocation(), CachedPlayerLoc);

	if (DistanceMoved > 200.f && TaskElapsedTime > 0.7f)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
}

