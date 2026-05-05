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
	APawn* Boss = OwnerComp.GetAIOwner()->GetPawn();
	APawn* Player = Cast<APawn>(BB->GetValueAsObject(ABossAIController::BB_TargetActor));
	if (!BB || !Boss || !Player) return EBTNodeResult::Failed;

	EBossAttackType Attack = (EBossAttackType)BB->GetValueAsEnum(ABossAIController::BB_SelectedAttack);

	if (Attack == EBossAttackType::MiasmaStep || Attack == EBossAttackType::None)
		return EBTNodeResult::Succeeded;

	TargetPosition = CalcTargetPosition(Attack, Boss, Player);

	return EBTNodeResult::InProgress;
}

void UBTTask_MoveToAttackPosition::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	APawn* Boss = OwnerComp.GetAIOwner()->GetPawn();
	if (!Boss) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

	// 이동 방향 구하기
	FVector Current = Boss->GetActorLocation();
	FVector Dir = (TargetPosition - Current);
	float Dist = Dir.Size();

	if (Dist <= AcceptanceRadius)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	// 페이즈에 따른 속도 변화
	AEnemyBoss* EBoss = Cast<AEnemyBoss>(Boss);
	int32 Phase = EBoss ? EBoss->GetCurrentPhase() : 1;
	float Speed = MoveSpeed + (Phase - 1) * 80.f;

	// 이동
	FVector NewLoc = Current + Dir.GetSafeNormal() * Speed * DeltaSeconds;
	Boss->SetActorLocation(NewLoc);

	// Boss를 플레이어 방향으로 회전
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (APawn* Player = Cast<APawn>(BB->GetValueAsObject(ABossAIController::BB_TargetActor)))
	{
		FRotator LookAt = (Player->GetActorLocation() - Boss->GetActorLocation()).Rotation();
		Boss->SetActorRotation(FRotator(0.f, LookAt.Yaw, 0.f));
	}
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