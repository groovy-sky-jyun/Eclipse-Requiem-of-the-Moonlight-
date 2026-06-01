// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_WraithChase.h"
#include "WraithAIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_WraithChase::UBTTask_WraithChase()
{
	NodeName = TEXT("Wraith Chase");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_WraithChase::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APawn* Wraith = OwnerComp.GetAIOwner()->GetPawn();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	APawn* Player = Cast<APawn>(BB->GetValueAsObject(AWraithAIController::BB_TargetActor));

	if (!Wraith || !BB || !Player) return EBTNodeResult::Failed;

	float Dist = FVector::Dist2D(Wraith->GetActorLocation(), Player->GetActorLocation());
	if (Dist <= AttackRange) return EBTNodeResult::Succeeded;
	
	return EBTNodeResult::InProgress;
}

void UBTTask_WraithChase::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	APawn* Wraith = OwnerComp.GetAIOwner()->GetPawn();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	APawn* Player = Cast<APawn>(BB->GetValueAsObject(AWraithAIController::BB_TargetActor));

	if (!Wraith || !BB || !Player)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	FVector WraithLoc = Wraith->GetActorLocation();
	FVector PlayerLoc = Player->GetActorLocation();

	float Dist = FVector::Dist2D(Wraith->GetActorLocation(), Player->GetActorLocation());
	if (Dist <= AttackRange) 
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	//플레이어 방향으로 직선 이동
	FVector Dir = (PlayerLoc - WraithLoc);
	Dir.Z = 0.f;
	Dir.Normalize();

	if (ACharacter* WraithChar = Cast<ACharacter>(Wraith))
	{
		WraithChar->AddMovementInput(Dir, 1.f);

		if (UCharacterMovementComponent* MoveComp = WraithChar->GetCharacterMovement())
		{
			MoveComp->MaxWalkSpeed = MoveSpeed;
		}
	}

	FRotator LookAt = Dir.Rotation();
	Wraith->SetActorRotation(FRotator(0.f, LookAt.Yaw, 0.f));
}

EBTNodeResult::Type UBTTask_WraithChase::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Aborted;
}
