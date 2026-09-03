// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAttack_DiscordDive.h"
#include "Eclipse.h"
#include "EnemyBoss.h"
#include "CombatInterface.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

namespace
{
	constexpr int32 TelegraphSegmentCount = 32;
	constexpr float TelegraphLifeTime = 0.05f;

	/** 겹친 원판을 띄우는 높이. 같은 평면이면 화면이 지글거린다. */
	constexpr float TelegraphLayerOffset = 1.f;
}

UBossAttack_DiscordDive::UBossAttack_DiscordDive()
{
	// 상승까지가 예열이다. 끝나는 시점을 스스로 정하므로 베이스 타이머를 쓰지 않는다.
	StartupTime = 0.f;
	RecoveryTime = 1.5f;
	MaxDuration = 8.f;
}

void UBossAttack_DiscordDive::OnStartup()
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

	UE_LOG(LogEclipse, Log, TEXT("[DiscordDive] Target locked"));

	// 낙하 지점은 캡슐 중심이다. 예고 원은 발밑에 깔아야 한다.
	TelegraphLocation = DiveTargetLocation;
	TelegraphLocation.Z -= Boss->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - TelegraphGroundOffset;

	DiveElapsedTime = 0.f;
	bDiving = true;

	MoveBoss(Boss->GetActorLocation(), GetLandingLocation(Player), DiveTime);

	SetAttackTimer(
		DiscordDiveTimer,
		FTimerDelegate::CreateWeakLambda(this, [this]()
		{
			bDiving = false;
			ApplyLandingDamage();
			EnterRecovery();
		}),
		DiveTime,
		false
	);
}

FVector UBossAttack_DiscordDive::GetLandingLocation(const APawn* Player) const
{
	AEnemyBoss* Boss = GetBoss();
	if (!Player || !IsValid(Boss)) return DiveTargetLocation;

	FVector ApproachDirection = DiveTargetLocation - AscendOrigin;
	ApproachDirection.Z = 0.f;

	FVector RetreatDirection = -ApproachDirection.GetSafeNormal();

	// 플레이어가 보스가 떠오른 자리에 서 있으면 물러설 방향이 없다.
	if (RetreatDirection.IsNearlyZero())
	{
		RetreatDirection = -Boss->GetActorForwardVector().GetSafeNormal2D();
	}

	// 판정 중심은 플레이어 자리에 두고, 보스만 캡슐이 겹치지 않을 만큼 물러선다.
	const float Clearance = Boss->GetSimpleCollisionRadius() + Player->GetSimpleCollisionRadius() + LandingClearance;

	return DiveTargetLocation + RetreatDirection * Clearance;
}

void UBossAttack_DiscordDive::OnTick(float DeltaTime)
{
	if (!bDiving) return;

	DiveElapsedTime += DeltaTime;

	DrawDiveTelegraph();
}

void UBossAttack_DiscordDive::DrawDiveTelegraph()
{
	const float Progress = (DiveTime > 0.f) ? FMath::Clamp(DiveElapsedTime / DiveTime, 0.f, 1.f) : 1.f;

	// 착지 직전에는 예고를 걷는다. 그때부터는 보스 본체가 낙하 지점을 알린다.
	if (Progress >= TelegraphFadeRatio) return;

	// 보스가 내려온 만큼 그림자가 커진다. 안쪽 빨강이 위로 덮인다.
	DrawTelegraphDisc(OuterRadius * Progress, FColor::Orange, 0.f);
	DrawTelegraphDisc(InnerRadius * Progress, FColor::Red, TelegraphLayerOffset);
}

void UBossAttack_DiscordDive::DrawTelegraphDisc(float Radius, const FColor& Color, float HeightOffset)
{
#if ENABLE_DRAW_DEBUG
	if (Radius <= 1.f) return;

	const FVector Center = TelegraphLocation + FVector(0.f, 0.f, HeightOffset);

	TArray<FVector> Vertices;
	Vertices.Reserve(TelegraphSegmentCount + 1);
	Vertices.Add(Center);

	for (int32 Segment = 0; Segment < TelegraphSegmentCount; ++Segment)
	{
		const float Angle = 2.f * PI * Segment / TelegraphSegmentCount;
		Vertices.Add(Center + FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.f) * Radius);
	}

	// 중심을 축으로 부채꼴을 이어 붙여 원판을 채운다.
	TArray<int32> Indices;
	Indices.Reserve(TelegraphSegmentCount * 3);

	for (int32 Segment = 0; Segment < TelegraphSegmentCount; ++Segment)
	{
		Indices.Add(0);
		Indices.Add(Segment + 1);
		Indices.Add((Segment + 1) % TelegraphSegmentCount + 1);
	}

	DrawDebugMesh(GetWorld(), Vertices, Indices, Color, false, TelegraphLifeTime);
#endif
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
	bDiving = false;

	// 취소나 워치독으로 끝나면 OnRecovery를 거치지 않으므로 한번더 호출해준다.
	RestoreMovement();
}
