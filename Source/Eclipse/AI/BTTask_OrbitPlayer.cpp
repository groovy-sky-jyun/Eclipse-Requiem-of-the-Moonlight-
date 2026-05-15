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

	int32 Phase = BB->GetValueAsInt(ABossAIController::BB_CurrentPhase);

	// 페이즈별 각도 스텝 설정
	float AngleStep = (Phase == 1) ? 30.f 
					: (Phase == 2) ? 50.f 
					: 70.f;

	// 3페이즈는 방향 무작위 (시계/반시계 랜덤)
	if (Phase == 3 && FMath::RandBool()) AngleStep = -AngleStep;

	float CurrentAngle = BB->GetValueAsFloat(ABossAIController::BB_OrbitAngle);
	float NewAngle = FMath::Fmod(CurrentAngle + AngleStep, 360.f);
	BB->SetValueAsFloat(ABossAIController::BB_OrbitAngle, NewAngle);

	// 목표 각도와 반경 저장
	CachedAngle = NewAngle;
	ElapsedTime = 0.f;

	float RadAngle = FMath::DegreesToRadians(NewAngle);
	FVector PlayerLocation = Player->GetActorLocation();

	// 페이즈별 타임아웃 설정
	TimeoutDuration = (Phase == 1) ? 3.f 
					: (Phase == 2) ? 2.5f 
					: 2.f;

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

	// 매 프레임 플레이어 위치 기준으로 목표 재계산
	// → 플레이어가 움직여도 보스가 항상 올바른 궤도 위치를 추적
	float RadAngle = FMath::DegreesToRadians(CachedAngle);
	FVector PlayerLoc = Player->GetActorLocation();
	float BossZ = Boss->GetActorLocation().Z; // 고도는 보스 현재 높이 유지

	FVector TargetPos = FVector(
		PlayerLoc.X + FMath::Cos(RadAngle) * OrbitRadius,
		PlayerLoc.Y + FMath::Sin(RadAngle) * OrbitRadius,
		BossZ  // Z 고정 → 3D 거리 오차 제거
	);

	FVector Current = Boss->GetActorLocation();

	// 2D 거리만 체크
	float Dist = FVector::Dist2D(Current, TargetPos);

	if (Dist <= AcceptanceRadius)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	int32 Phase = BB->GetValueAsInt(ABossAIController::BB_CurrentPhase);
	float Speed = MoveSpeed + (Phase - 1) * 80.f;

	FVector Dir = TargetPos - Current;
	Dir.Z = 0.f; // 수직 이동 방지
	Dir.Normalize();

	Boss->SetActorLocation(Current + Dir * Speed * DeltaSeconds);

	// 플레이어를 바라보도록 회전
	FRotator LookAt = (Player->GetActorLocation() - Boss->GetActorLocation()).Rotation();
	Boss->SetActorRotation(FRotator(0.f, LookAt.Yaw, 0.f));
}

EBTNodeResult::Type UBTTask_OrbitPlayer::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Aborted;
}
