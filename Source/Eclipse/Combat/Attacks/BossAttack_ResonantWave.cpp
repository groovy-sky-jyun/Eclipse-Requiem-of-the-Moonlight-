// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAttack_ResonantWave.h"
#include "Eclipse.h"
#include "EnemyBoss.h"
#include "CombatInterface.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

UBossAttack_ResonantWave::UBossAttack_ResonantWave()
{
	// 연주 0.4 + 거문고 배치 1.2
	WindupTime = 1.6f;
	RecoveryTime = 1.8f;
	MaxDuration = 12.f;
}

void UBossAttack_ResonantWave::OnWindup()
{
	// 거문고 6대를 원형으로 세우는 배치 연출 자리
}

void UBossAttack_ResonantWave::OnActive()
{
	AEnemyBoss* Boss = GetBoss();
	if (!IsValid(Boss)) return;

	// 확산 중심
	WaveOrigin = Boss->GetActorLocation();
	CurrentWaveIndex = 0;

	StartNextWave();
}

void UBossAttack_ResonantWave::StartNextWave()
{
	if (CurrentWaveIndex >= WaveCount)
	{
		EnterRecovery();
		return;
	}

	// 파가 진행될수록 더 멀리 퍼진다. 
	CurrentWaveMaxRadius = FMath::Max(FirstWaveRadius + WaveRadiusStep * CurrentWaveIndex, InitRingRadius);

	// 음파는 거문고가 늘어선 원에서 출발한다.
	CurrentWaveRadius = InitRingRadius;
	CurrentWaveSpeed = (CurrentWaveMaxRadius - InitRingRadius) / WaveDuration;
	bWaveHit = false;

	UE_LOG(LogEclipse, Log, TEXT("[ResonantWave] Wave %d/%d (radius %.0f)"), CurrentWaveIndex + 1, WaveCount, CurrentWaveMaxRadius);

	SetAttackTimer(
		ResonantWaveTimer,
		FTimerDelegate::CreateWeakLambda(this, [this]()
		{
			++CurrentWaveIndex;
			StartNextWave();
		}),
		WaveDuration + WaveInterval,
		false
	);
}

void UBossAttack_ResonantWave::OnTick(float DeltaTime)
{
	if (GetAttackState() != EBossAttackState::Active) return;
	if (CurrentWaveRadius >= CurrentWaveMaxRadius) return;

	CurrentWaveRadius = FMath::Min(CurrentWaveRadius + CurrentWaveSpeed * DeltaTime, CurrentWaveMaxRadius);

	CheckWaveHit();

#if ENABLE_DRAW_DEBUG
	DrawDebugCircle(GetWorld(), WaveOrigin, CurrentWaveRadius, 48, FColor::Red, false, 0.05f, 0, 6.f,
		FVector(1.f, 0.f, 0.f), FVector(0.f, 1.f, 0.f), false);
#endif
}

void UBossAttack_ResonantWave::CheckWaveHit()
{
	if (bWaveHit) return;

	APawn* Player = GetTargetPlayer();
	if (!Player) return;
	if (!Player->Implements<UCombatInterface>()) return;

	const float Distance = FVector::Dist2D(Player->GetActorLocation(), WaveOrigin);

	// 링의 바깥 경계와 안쪽 경계 사이에 있으면 맞는다.
	if (Distance > CurrentWaveRadius) return;
	if (Distance < CurrentWaveRadius - WaveThickness) return;

	ICombatInterface::Execute_TakeCombatDamage(Player, WaveDamage, GetBoss());
	bWaveHit = true;

	UE_LOG(LogEclipse, Log, TEXT("[ResonantWave] Hit : wave %d"), CurrentWaveIndex + 1);
}

void UBossAttack_ResonantWave::OnRecovery()
{
}

void UBossAttack_ResonantWave::OnCancel()
{
}

void UBossAttack_ResonantWave::OnFinish()
{
}
