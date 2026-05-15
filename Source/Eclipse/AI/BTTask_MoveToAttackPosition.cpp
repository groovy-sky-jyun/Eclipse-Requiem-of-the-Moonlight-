// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_MoveToAttackPosition.h"
#include "BossAIController.h"
#include "EnemyBoss.h"
#include "BossAttack.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_MoveToAttackPosition::UBTTask_MoveToAttackPosition()
{
	NodeName = TEXT("Move To Attack Position");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_MoveToAttackPosition::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AEnemyBoss* Boss = Cast<AEnemyBoss>(OwnerComp.GetAIOwner()->GetPawn());
	APawn* Player = Cast<APawn>(BB->GetValueAsObject(ABossAIController::BB_TargetActor));
	if (!BB || !Boss || !Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MoveToAttackPosition] is Failed : line23"));
		return EBTNodeResult::Failed;
	}

	/*
	EBossAttackType Attack = (EBossAttackType)BB->GetValueAsEnum(ABossAIController::BB_SelectedAttack);

	if (Attack == EBossAttackType::MiasmaStep || Attack == EBossAttackType::None)
		return EBTNodeResult::Succeeded;

	TargetPosition = CalcTargetPosition(Attack, Boss, Player);
	*/

	FVector PlayerLoc = Player->GetActorLocation();
	TargetPosition = FVector(PlayerLoc.X, PlayerLoc.Y, Boss->GetActorLocation().Z);

	return EBTNodeResult::InProgress;
}

void UBTTask_MoveToAttackPosition::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AEnemyBoss* Boss = Cast<AEnemyBoss>(OwnerComp.GetAIOwner()->GetPawn());
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	APawn* Player = Cast<APawn>(BB->GetValueAsObject(ABossAIController::BB_TargetActor));
	if (!Boss || !BB || !Player) 
	{ 
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed); 
		UE_LOG(LogTemp, Warning, TEXT("[MoveToAttackPosition] is Failed : line46"));
		return; 
	}

	FVector PlayerLoc = Player->GetActorLocation();
	TargetPosition = FVector(PlayerLoc.X, PlayerLoc.Y, Boss->GetActorLocation().Z);

	// 이동 방향 구하기
	FVector Current = Boss->GetActorLocation();
	float Dist = FVector::Dist2D(Current, TargetPosition); // XY 거리만 체크

	if (Dist <= AcceptanceRadius)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	// 페이즈에 따른 속도 변화
	int32 Phase = Boss ? Boss->GetCurrentPhase() : 1;
	float Speed = MoveSpeed + (Phase - 1) * 80.f;

	FVector Dir = (TargetPosition - Current);
	Dir.Z = 0.f; // 수직 이동 방지
	Dir.Normalize();

	Boss->SetActorLocation(Current + Dir * Speed * DeltaSeconds);

	if (Player)
	{
		FRotator LookAt = (Player->GetActorLocation() - Boss->GetActorLocation()).Rotation();
		Boss->SetActorRotation(FRotator(0.f, LookAt.Yaw, 0.f));
	}

	/* 기존 공격 타입별 위치 계산 코드 (추후 활성화)
	EBossAttack Attack = (EBossAttack)BB->GetValueAsEnum(ABossAIController::BB_SelectedAttack);
	if (Attack == EBossAttack::MiasmaStep || Attack == EBossAttack::None)
		return EBTNodeResult::Succeeded;
	TargetPosition = CalcTargetPosition(Attack, Boss, Player);
	*/
}

EBTNodeResult::Type UBTTask_MoveToAttackPosition::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Aborted;
}

FVector UBTTask_MoveToAttackPosition::CalcTargetPosition(EBossAttackType Attack, APawn* Boss, APawn* Player) const
{
	FVector PlayerLoc = Player->GetActorLocation();
	FVector BossLoc = Boss->GetActorLocation();

	FVector ToBoss2D = (BossLoc - PlayerLoc);
	ToBoss2D.Z = 0.f;
	ToBoss2D = ToBoss2D.GetSafeNormal();
	if (ToBoss2D.IsNearlyZero()) ToBoss2D = FVector(1.f, 0.f, 0.f);

	switch (Attack)
	{
	case EBossAttackType::BloodBolt:
		return PlayerLoc + ToBoss2D * 600.f + FVector(0.f, 0.f, BossLoc.Z);

	case EBossAttackType::WraithDrop:

	case EBossAttackType::EclipseVeil:

	case EBossAttackType::LunarBeam:

	case EBossAttackType::DamningTether:

	default:
		return BossLoc;
	}
}