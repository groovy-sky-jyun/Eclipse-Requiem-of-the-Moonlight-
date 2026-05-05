// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_FindPlayer.h"
#include "BossAIController.h"
#include "EnemyBoss.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UBTService_FindPlayer::UBTService_FindPlayer()
{
	NodeName = TEXT("Find Player");
	
	// Tick Node Time Cycle
	Interval = 0.1f;
	RandomDeviation = 0.05f;
}

void UBTService_FindPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	APawn* Boss = OwnerComp.GetAIOwner()->GetPawn();
	if (!BB || !Boss) return;

	APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!Player) return;

	BB->SetValueAsObject(ABossAIController::BB_TargetActor, Player);

	float Dist = FVector::Dist(Boss->GetActorLocation(), Player->GetActorLocation());
	BB->SetValueAsFloat(ABossAIController::BB_DistanceToTarget, Dist);
}
