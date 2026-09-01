// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAttack_DarkSweep.h"
#include "Eclipse.h"
#include "EnemyBoss.h"
#include "Attack_Marker.h"
#include "CombatInterface.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"

void UBossAttack_DarkSweep::OnWindup()
{
	AEnemyBoss* Boss = GetBoss();

	APawn* Player = GetTargetPlayer();
	if (!Player) return;

	// 돌진 방향
	FVector ToPlayer = (Player->GetActorLocation() - Boss->GetActorLocation());
	ToPlayer.Z = 0.f;
	DarkSweepDirection = ToPlayer.GetSafeNormal();

	// 돌진 시작/끝 위치
	FVector Origin = Boss->GetActorLocation();
	Origin.Z = Player->GetActorLocation().Z + DarkSweepHeight;

	DarkSweepStartLoc = Origin;
	DarkSweepEndLoc = Origin + DarkSweepDirection * DarkSweepDistance;

	bDarkSweepHit = false;

	DarkSweep_StartTelegraph();
}

		// 1. DarkSweep : Marker 표시
void UBossAttack_DarkSweep::DarkSweep_StartTelegraph()
{
	AEnemyBoss* Boss = GetBoss();

	if (AttackMarkerClass)
	{
		FVector MarkerCenter = (DarkSweepStartLoc + DarkSweepEndLoc) * 0.5f;
		MarkerCenter.Z = DarkSweepStartLoc.Z;

		FRotator MarkerRot = DarkSweepDirection.Rotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = Boss;

		AAttack_Marker* Marker = GetWorld()->SpawnActor<AAttack_Marker>(AttackMarkerClass, MarkerCenter, MarkerRot, SpawnParams);
		if(Marker)
		{
			Marker->SetRectMarker(DarkSweepDistance, DarkSweepHalfWidth * 2, 0.8);
		}
	}

	SetAttackTimer(
		DarkSweepTimer,
		FTimerDelegate::CreateUObject(this, &UBossAttack_DarkSweep::DarkSweep_StartDash),
		0.8f,
		false
	);
}

		// 2. DarkSweep : 돌진
void UBossAttack_DarkSweep::DarkSweep_StartDash()
{
	AEnemyBoss* Boss = GetBoss();

	// 텔레그래프가 끝났다. 여기부터 돌진 판정이 나간다.
	EnterActive();

	/* 보스에 부착되는 잔상 이펙트
	if (NS_DarkSweepTrail)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			NS_DarkSweepTrail,
			Boss->GetMesh(),     // 보스 메쉬에 부착
			NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget,
			true                 // 이펙트 종료 시 자동 제거
		);
	}*/

	if (UCharacterMovementComponent* MovementComponent = Boss->GetCharacterMovement())
	{
		MovementComponent->SetMovementMode(MOVE_Flying);
		MovementComponent->GravityScale = 0.f;
	}

	Boss->SetActorLocation(DarkSweepStartLoc);
	Boss->SetActorRotation(DarkSweepDirection.Rotation());

	// 거리 / 속도 = 돌진 소요 시간
	float DashDuration = DarkSweepDistance / DarkSweepSpeed;
	const int32 Steps = 12;
	float StepTime = DashDuration / Steps;

	for (int32 i = 1; i < Steps; i++)
	{
		float Alpha = (float)i / Steps;
		FVector StepLoc = FMath::Lerp(DarkSweepStartLoc, DarkSweepEndLoc, Alpha);

		FTimerHandle StepHandle;
		FTimerDelegate Delegate = FTimerDelegate::CreateWeakLambda(this, [this, StepLoc]()
		{
			AEnemyBoss* StepBoss = GetBoss();
			if (!IsValid(StepBoss)) return;

			StepBoss->SetActorLocation(StepLoc);

			DarkSweep_CheckHit(StepLoc);
		});
		SetAttackTimer(StepHandle, Delegate, StepTime * i, false);
	}

	SetAttackTimer(
		DarkSweepTimer,
		FTimerDelegate::CreateWeakLambda(this, [this]()
		{
			EnterRecovery();
		}),
		DashDuration,
		false
	);
}

		// 3. DarkSweep : 충돌 감지
void UBossAttack_DarkSweep::DarkSweep_CheckHit(const FVector& CurrentStepLoc)
{
	if (bDarkSweepHit) return;

	APawn* Player = GetTargetPlayer();
	if (!Player) return;

	FVector ToPlayer = Player->GetActorLocation() - CurrentStepLoc;
	float FwdDot = FVector::DotProduct(ToPlayer, DarkSweepDirection);
	FVector Lateral = ToPlayer - DarkSweepDirection * FwdDot;
	float LatDist = Lateral.Size2D();

	// 전방 범위 내 + 폭 범위 내 
	// #####[**따로 옵시디언에 정리**]
	if (FwdDot >= -200.f && FwdDot <= 200.f && LatDist <= DarkSweepHalfWidth)
	{
		if (Player->Implements<UCombatInterface>())
		{
			ICombatInterface::Execute_TakeCombatDamage(
				Player, DarkSweepDamage, GetBoss());
			bDarkSweepHit = true;

			UE_LOG(LogEclipse, Log,TEXT("[DarkSweep] Hit"));

			// 플레이어 넉백 적용
			// Player->ApplyKnockback(DarkSweepDirection, 800.f);
		}
	}
}

		// 4. DarkSweep : 종료 (돌진 상태 원복)
void UBossAttack_DarkSweep::OnFinish()
{
	AEnemyBoss* Boss = GetBoss();
	if (!IsValid(Boss)) return;

	if (UCharacterMovementComponent* MovementComponent = Boss->GetCharacterMovement())
	{
		MovementComponent->SetMovementMode(MOVE_Walking);
		MovementComponent->GravityScale = 1.f;
	}
}
