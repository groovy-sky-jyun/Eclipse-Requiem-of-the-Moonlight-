// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_IdleHover.h"
#include "BossAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_IdleHover::UBTTask_IdleHover()
{
	NodeName = TEXT("Idle Hover");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_IdleHover::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ElapsedTime = 0.f;
	return EBTNodeResult::InProgress;
}

void UBTTask_IdleHover::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	/*
	APawn* Boss = OwnerComp.GetAIOwner()->GetPawn();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!Boss || !BB) return;

	ElapsedTime += DeltaSeconds;

	FVector HomeLoc = BB->GetValueAsVector(ABossAIController::BB_BossInitLocation);
	float DeltaZ = FMath::Sin(ElapsedTime * HoverSpeed) * HoverAmplitude;
	FVector Target = HomeLoc + FVector(0.f, 0.f, DeltaZ);

	Boss->SetActorLocation(Target);
	*/
}
