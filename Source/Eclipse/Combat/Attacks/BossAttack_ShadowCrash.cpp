// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAttack_ShadowCrash.h"
#include "Eclipse.h"
#include "EnemyBoss.h"
#include "Attack_Marker.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "CombatInterface.h"
#include "Engine/OverlapResult.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

void UBossAttack_ShadowCrash::OnStartup()
{
	ShadowCrashOriginLoc = GetBoss()->GetActorLocation();

	ShadowCrash_StartAscend();
}

		// 1. ShadowCrash : 상승
void UBossAttack_ShadowCrash::ShadowCrash_StartAscend()
{	
	AEnemyBoss* Boss = GetBoss();

	FVector RiseTarget = Boss->GetActorLocation() + FVector(0.f, 0.f, 300.f);

	const float AscendDuration = 0.8f;
	const int32 Steps = 16;
	float StepTime = AscendDuration / Steps;

	if (UCharacterMovementComponent* MovementComponent = Boss->GetCharacterMovement())
	{
		MovementComponent->SetMovementMode(MOVE_Flying);
		MovementComponent->GravityScale = 0.f;
	}

	for (int32 i = 1; i <= Steps; i++)
	{
		float Alpha = (float)i / Steps;
		FVector StepLoc = FMath::Lerp(ShadowCrashOriginLoc, RiseTarget, Alpha);

		FTimerHandle StepHandle;
		FTimerDelegate Delegate = FTimerDelegate::CreateWeakLambda(this, [this, StepLoc]()
		{
			AEnemyBoss* StepBoss = GetBoss();
			if (IsValid(StepBoss)) StepBoss->SetActorLocation(StepLoc, false, nullptr, ETeleportType::TeleportPhysics);
		});
		SetAttackTimer(StepHandle, Delegate, StepTime * i, false);
	}

	SetAttackTimer(
		ShadowCrashTimer,
		FTimerDelegate::CreateUObject(this, &UBossAttack_ShadowCrash::ShadowCrash_StartTelegraph),
		AscendDuration,
		false
	);
}

		// 2. ShadowCrash : Warning Marker
void UBossAttack_ShadowCrash::ShadowCrash_StartTelegraph()
{
	AEnemyBoss* Boss = GetBoss();

	APawn* Player = GetTargetPlayer();
	if (!Player) return;

	ShadowCrashTargetLoc = Player->GetActorLocation();
	ShadowCrashTargetLoc.Z = 0.f;

	if (AttackMarkerClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = Boss;
		AAttack_Marker* Marker = GetWorld()->SpawnActor<AAttack_Marker>(
			AttackMarkerClass,
			ShadowCrashTargetLoc,
			FRotator::ZeroRotator,
			SpawnParams
		);

		if (Marker)
		{
			// 예고 표시와 실제 판정은 같은 값에서 나와야 한다.
			// 따로 두면 "표시된 원 밖에서 맞았다"가 반드시 생긴다.
			Marker->SetCircleMarker(ShadowCrashMarkerRadius, 2.f);
		}
	}
	else
	{
		UE_LOG(LogEclipse, Error, TEXT("ShadowCrash Warning Marker is Null"));
	}

	SetAttackTimer(
		ShadowCrashTimer,
		FTimerDelegate::CreateUObject(this, &UBossAttack_ShadowCrash::ShadowCrash_StartDive),
		0.3f,
		false
	);
}

		// 3. ShadowCrash : 강하
void UBossAttack_ShadowCrash::ShadowCrash_StartDive()
{
	AEnemyBoss* Boss = GetBoss();

	// 상승과 텔레그래프가 끝났다. 여기부터 낙하 판정이 나간다.
	EnterActive();

	FVector DiveStart = Boss->GetActorLocation();
	FVector DiveEnd = ShadowCrashTargetLoc;

	const float DiveDuration = 0.8f;
	const int32 Steps = 10;
	float StepTime = DiveDuration / Steps;

	for (int32 i = 1; i <= Steps; i++)
	{
		float Alpha = (float)i / Steps;
		FVector StepLoc = FMath::Lerp(DiveStart, DiveEnd, Alpha);

		FTimerHandle StepHandle;
		FTimerDelegate Delegate = FTimerDelegate::CreateWeakLambda(this, [this, StepLoc]()
		{
			AEnemyBoss* StepBoss = GetBoss();
			if (IsValid(StepBoss)) StepBoss->SetActorLocation(StepLoc);
		});
		SetAttackTimer(StepHandle, Delegate, StepTime * i, false);
	}


	SetAttackTimer(
		ShadowCrashTimer,
		FTimerDelegate::CreateUObject(this, &UBossAttack_ShadowCrash::ShadowCrash_OnImpact),
		DiveDuration,
		false
	);
}

		// 4. ShadowCrash : 충돌 판정
void UBossAttack_ShadowCrash::ShadowCrash_OnImpact()
{
	AEnemyBoss* Boss = GetBoss();

	if (UCharacterMovementComponent* MovementComponent = Boss->GetCharacterMovement())
	{
		MovementComponent->GravityScale = 1.f;
		MovementComponent->SetMovementMode(MOVE_Walking);
	}

	const FVector ImpactLoc = Boss->GetActorLocation();

	if (NS_ShadowCrashLand)
	{
		FVector CrashLoc = ImpactLoc;
		CrashLoc.Z = 0.f;

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NS_ShadowCrashLand, CrashLoc);
	}

	// 착지 지점 원형 판정. 팀 구분은 맞는 쪽(ABaseCharacter::HandleTakeDamage)이 한다.
	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(ShadowCrashImpact), false, Boss);

	GetWorld()->OverlapMultiByChannel(
		Overlaps,
		ImpactLoc,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(ShadowCrashMarkerRadius),
		QueryParams);

	// 한 액터가 캡슐과 메시로 각각 잡히면 두 번 맞는다.
	TSet<AActor*> DamagedActors;

	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* HitActor = Overlap.GetActor();
		if (!HitActor || !HitActor->Implements<UCombatInterface>()) continue;

		bool bAlreadyDamaged = false;
		DamagedActors.Add(HitActor, &bAlreadyDamaged);
		if (bAlreadyDamaged) continue;

		ICombatInterface::Execute_TakeCombatDamage(HitActor, ShadowCrashDamage, Boss);
	}

#if ENABLE_DRAW_DEBUG
	DrawDebugSphere(GetWorld(), ImpactLoc, ShadowCrashMarkerRadius, 16, FColor::Red, false, 1.5f);
#endif

	EnterRecovery();

}

