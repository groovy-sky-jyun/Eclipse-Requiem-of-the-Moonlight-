// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_CombatHover.h"
#include "BossAIController.h"
#include "EnemyBoss.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_CombatHover::UBTTask_CombatHover()
{
	NodeName = TEXT("Combat Hover");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_CombatHover::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	APawn* Boss = OwnerComp.GetAIOwner()->GetPawn();
	if (!BB || !Boss) return EBTNodeResult::Failed;

	ElapsedTime = 0.f;
	StartLocation = Boss->GetActorLocation();

	int32 Phase = BB->GetValueAsInt(ABossAIController::BB_CurrentPhase);
	HoverDuration = (Phase == 1) ? 2.0f : (Phase == 2) ? 1.2f : 0.6f;

	return EBTNodeResult::InProgress;
}

void UBTTask_CombatHover::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    APawn* Boss = OwnerComp.GetAIOwner()->GetPawn();
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!Boss || !BB) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

    ElapsedTime += DeltaSeconds;

    /* 위아래 부유
    float DeltaZ = FMath::Sin(ElapsedTime * HoverSpeed) * HoverAmplitude;
    FVector NewLoc = StartLocation;
    NewLoc.Z += DeltaZ;
    Boss->SetActorLocation(NewLoc);
    */

    // 플레이어에게 방향 회전
    if (APawn* Player = Cast<APawn>(BB->GetValueAsObject(ABossAIController::BB_TargetActor)))
    {
        FRotator LookAt = (Player->GetActorLocation() - Boss->GetActorLocation()).Rotation();
        Boss->SetActorRotation(FRotator(0.f, LookAt.Yaw, 0.f));
    }

    // 대기 시간 완료
    if (ElapsedTime >= HoverDuration)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}

EBTNodeResult::Type UBTTask_CombatHover::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Aborted;
}
