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
	AEnemyBoss* Boss = Cast<AEnemyBoss>(OwnerComp.GetAIOwner()->GetPawn());
	APawn* Player = Cast<APawn>(BB->GetValueAsObject(ABossAIController::BB_TargetActor));
	if (!BB || !Boss || !Player) return EBTNodeResult::Failed;

	ElapsedTime = 0.f;

	float Dist2D = FVector::Dist2D(Boss->GetActorLocation(), Player->GetActorLocation());

	if (Dist2D <= CloseRangeThreshold)
	{
		return EBTNodeResult::Succeeded;
	}
	else
	{
		FVector ArenaCenter = BB->GetValueAsVector(ABossAIController::BB_BossInitLocation);
		TargetPosition = CalcStrafeTarget(Boss, Player, ArenaCenter);
	}

	return EBTNodeResult::InProgress;
}

void UBTTask_OrbitPlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AEnemyBoss* Boss = Cast<AEnemyBoss>(OwnerComp.GetAIOwner()->GetPawn());
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	APawn* Player = Cast<APawn>(BB->GetValueAsObject(ABossAIController::BB_TargetActor));
	if (!Boss || !BB || !Player) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

	ElapsedTime += DeltaSeconds;

	// 타임아웃 — 도달 못해도 다음 Task로 넘어감
	if (ElapsedTime >= TimeoutDuration)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	FVector Current = Boss->GetActorLocation();

	// 거리 도달 체크
	if (FVector::Dist2D(Current, TargetPosition) <= AcceptanceRadius)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	// 페이즈에 따라 속도 가산
	AEnemyBoss* EBoss = Cast<AEnemyBoss>(Boss);
	int32 Phase = EBoss ? EBoss->GetCurrentPhase() : 1;
	float Speed = MoveSpeed + (Phase - 1) * 100.f;

	// 이동 방향 (Z 고정)
	FVector Dir = (TargetPosition - Current);
	Dir.Z = 0.f;
	Dir.Normalize();

	Boss->SetActorLocation(Current + Dir * Speed * DeltaSeconds);

	// 플레이어 방향 바라보기
	FRotator LookAt = (Player->GetActorLocation() - Boss->GetActorLocation()).Rotation();
	Boss->SetActorRotation(FRotator(0.f, LookAt.Yaw, 0.f));
}

EBTNodeResult::Type UBTTask_OrbitPlayer::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Aborted;
}

FVector UBTTask_OrbitPlayer::CalcStrafeTarget(APawn* Boss, APawn* Player, const FVector& ArenaCenter) const
{
	FVector BossLoc = Boss->GetActorLocation();
	FVector PlayerLoc = Player->GetActorLocation();

	FVector ToPlayer2D = (PlayerLoc - BossLoc);
	ToPlayer2D.Z = 0.f;
	ToPlayer2D.Normalize();

	// 방향 확인해보기.
	FVector LeftVec = FVector::CrossProduct(ToPlayer2D, FVector::UpVector).GetSafeNormal();
	FVector RightVec = -LeftVec;

	// 대각선 방향 구하기
	FVector DiagonalRightDir = (ToPlayer2D + RightVec).GetSafeNormal();
	FVector DiagonalLeftDir = (ToPlayer2D + LeftVec).GetSafeNormal();

	FVector CandidateRight = BossLoc + (DiagonalRightDir * StrafeDistance);
	FVector CandidateLeft = BossLoc + (DiagonalLeftDir * StrafeDistance);

	float DistRight = FVector::Dist2D(CandidateRight, ArenaCenter);
	float DistLeft = FVector::Dist2D(CandidateLeft, ArenaCenter);

	FVector ChosenTarget = (DistRight <= DistLeft) ? CandidateRight : CandidateLeft;
	ChosenTarget.Z = BossLoc.Z;

	return ChosenTarget;
}

