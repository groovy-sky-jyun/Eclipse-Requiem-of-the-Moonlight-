// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/BTTask_OrbitPlayer.h"
#include "BossAIController.h"
#include "EnemyBoss.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_OrbitPlayer::UBTTask_OrbitPlayer()
{
	NodeName = TEXT("Orbit Player");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_OrbitPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	APawn* Boss = OwnerComp.GetAIOwner()->GetPawn();
	APawn* Player = Cast<APawn>(BB->GetValueAsObject(ABossAIController::BB_TargetActor));
	if (!BB || !Boss || !Player) return EBTNodeResult::Failed;

	int32 Phase = BB->GetValueAsInt(ABossAIController::BB_CurrentPhase);

	// 페이즈별 각도 스텝 설정
	float AngleStep = (Phase == 1) ? 30.f : (Phase == 2) ? 50.f : 70.f;

	// 3페이즈는 방향 무작위 (시계/반시계 랜덤)
	if (Phase == 3 && FMath::RandBool()) AngleStep = -AngleStep;

	float CurrentAngle = BB->GetValueAsFloat(ABossAIController::BB_OrbitAngle);
	float NewAngle = FMath::Fmod(CurrentAngle + AngleStep, 360.f);
	BB->SetValueAsFloat(ABossAIController::BB_OrbitAngle, NewAngle);

	float RadAngle = FMath::DegreesToRadians(NewAngle);
	FVector PlayerLocation = Player->GetActorLocation();

	// 페이즈별 고도 변화
	float HeightOffset = (Phase == 1) ? 400.f
		: (Phase == 2) ? FMath::RandRange(300.f, 550.f)
		: FMath::RandRange(250.f, 650.f);

	// 목표 위치 계산 (플레이어 중심)
	OrbitTargetPos = PlayerLocation + FVector(FMath::Cos(RadAngle) * OrbitRadius, FMath::Sin(RadAngle) * OrbitRadius, HeightOffset);

	return EBTNodeResult::InProgress;
}

void UBTTask_OrbitPlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	APawn* Boss = OwnerComp.GetAIOwner()->GetPawn();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!Boss || !BB) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

	FVector Current = Boss->GetActorLocation();
	float Dist = FVector::Dist(Current, OrbitTargetPos);

	if (Dist <= AcceptanceRadius)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	int32 Phase = BB->GetValueAsInt(ABossAIController::BB_CurrentPhase);
	float Speed = MoveSpeed + (Phase - 1) * 80.f;
	FVector Dir = (OrbitTargetPos - Current).GetSafeNormal();
	Boss->SetActorLocation(Current + Dir * Speed * DeltaSeconds);

	// 플레이어를 바라보도록 회전
	if (APawn* Player = Cast<APawn>(BB->GetValueAsObject(ABossAIController::BB_TargetActor)))
	{
		FRotator LookAt = (Player->GetActorLocation() - Boss->GetActorLocation()).Rotation();
		Boss->SetActorRotation(FRotator(0.f, LookAt.Yaw, 0.f));
	}
}

EBTNodeResult::Type UBTTask_OrbitPlayer::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Aborted;
}
