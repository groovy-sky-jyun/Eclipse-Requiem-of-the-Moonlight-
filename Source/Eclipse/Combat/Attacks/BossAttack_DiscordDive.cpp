// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAttack_DiscordDive.h"
#include "Eclipse.h"
#include "EnemyBoss.h"
#include "CombatInterface.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

UBossAttack_DiscordDive::UBossAttack_DiscordDive()
{
	// 상승까지가 예열이다. 끝나는 시점을 스스로 정하므로 베이스 타이머를 쓰지 않는다.
	WindupTime = 0.f;
	RecoveryTime = 1.5f;
	MaxDuration = 8.f;
}

void UBossAttack_DiscordDive::OnWindup()
{
	// 박쥐 떼가 날아와 보스에게 붙는다. 이게 상승 수단이라 뜨기 전에 온다.
	SetAttackTimer(
		DiscordDiveTimer,
		FTimerDelegate::CreateUObject(this, &UBossAttack_DiscordDive::Ascend),
		BatGatherTime,
		false
	);
}

void UBossAttack_DiscordDive::Ascend()
{
	AEnemyBoss* Boss = GetBoss();
	if (!IsValid(Boss)) return;

	AscendOrigin = Boss->GetActorLocation();

	if (UCharacterMovementComponent* Movement = Boss->GetCharacterMovement())
	{
		Movement->SetMovementMode(MOVE_Flying);
		Movement->GravityScale = 0.f;
	}

	MoveBoss(AscendOrigin, AscendOrigin + FVector(0.f, 0.f, AscendHeight), AscendTime);

	// 상승 뒤 상공에 머물다 판정을 연다.
	SetAttackTimer(
		DiscordDiveTimer,
		FTimerDelegate::CreateWeakLambda(this, [this]() { EnterActive(); }),
		AscendTime + HoverTime,
		false
	);
}

void UBossAttack_DiscordDive::OnActive()
{
	Dive();
}

void UBossAttack_DiscordDive::Dive()
{
	AEnemyBoss* Boss = GetBoss();
	if (!IsValid(Boss)) return;

	// 플레이어를 놓치면 떠오른 자리로 내려온다. 공중에 남지 않게 한다.
	APawn* Player = GetTargetPlayer();
	DiveTargetLocation = Player ? Player->GetActorLocation() : AscendOrigin;
	DiveTargetLocation.Z = AscendOrigin.Z;

#if ENABLE_DRAW_DEBUG
	DrawDebugSphere(GetWorld(), DiveTargetLocation, InnerRadius, 24, FColor::Red, false, DiveTime);
	DrawDebugSphere(GetWorld(), DiveTargetLocation, OuterRadius, 24, FColor::Orange, false, DiveTime);
#endif

	UE_LOG(LogEclipse, Log, TEXT("[DiscordDive] Target locked"));

	MoveBoss(Boss->GetActorLocation(), DiveTargetLocation, DiveTime);

	SetAttackTimer(
		DiscordDiveTimer,
		FTimerDelegate::CreateWeakLambda(this, [this]()
		{
			ApplyLandingDamage();
			EnterRecovery();
		}),
		DiveTime,
		false
	);
}

void UBossAttack_DiscordDive::ApplyLandingDamage()
{
	APawn* Player = GetTargetPlayer();
	if (!Player) return;
	if (!Player->Implements<UCombatInterface>()) return;

	const float Distance = FVector::Dist2D(Player->GetActorLocation(), DiveTargetLocation);
	if (Distance > OuterRadius) return;

	const float Damage = (Distance <= InnerRadius) ? InnerDamage : OuterDamage;

	ICombatInterface::Execute_TakeCombatDamage(Player, Damage, GetBoss());

	UE_LOG(LogEclipse, Log, TEXT("[DiscordDive] Hit : %.0f (dist %.0f)"), Damage, Distance);
}

void UBossAttack_DiscordDive::MoveBoss(const FVector& From, const FVector& To, float Duration)
{
	const float StepTime = Duration / MoveSteps;

	for (int32 Step = 1; Step <= MoveSteps; ++Step)
	{
		const FVector StepLocation = FMath::Lerp(From, To, (float)Step / MoveSteps);

		FTimerHandle StepTimer;
		SetAttackTimer(
			StepTimer,
			FTimerDelegate::CreateWeakLambda(this, [this, StepLocation]()
			{
				AEnemyBoss* Boss = GetBoss();
				if (!IsValid(Boss)) return;

				Boss->SetActorLocation(StepLocation);
			}),
			StepTime * Step,
			false
		);
	}
}

void UBossAttack_DiscordDive::RestoreMovement()
{
	AEnemyBoss* Boss = GetBoss();
	if (!IsValid(Boss)) return;

	UCharacterMovementComponent* Movement = Boss->GetCharacterMovement();
	if (!Movement || Movement->MovementMode != MOVE_Flying) return;

	Movement->SetMovementMode(MOVE_Walking);
	Movement->GravityScale = 1.f;
}

void UBossAttack_DiscordDive::OnRecovery()
{
	RestoreMovement();
}

void UBossAttack_DiscordDive::OnCancel()
{
}

void UBossAttack_DiscordDive::OnFinish()
{
	// 취소나 워치독으로 끝나면 OnRecovery를 거치지 않으므로 한번더 호출해준다.
	RestoreMovement();
}
