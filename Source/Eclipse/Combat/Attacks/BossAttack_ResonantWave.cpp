// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAttack_ResonantWave.h"
#include "Eclipse.h"
#include "EnemyBoss.h"
#include "CombatInterface.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

namespace
{
	constexpr int32 WaveSegmentCount = 48;
	constexpr float WaveLifeTime = 0.05f;
}

UBossAttack_ResonantWave::UBossAttack_ResonantWave()
{
	// 연주 0.4 + 거문고 배치 1.2
	StartupTime = 1.6f;
	RecoveryTime = 1.8f;
	MaxDuration = 12.f;
}

void UBossAttack_ResonantWave::OnStartup()
{
	// 거문고 6대를 원형으로 세우는 배치 연출 자리
}

void UBossAttack_ResonantWave::OnActive()
{
	AEnemyBoss* Boss = GetBoss();
	if (!IsValid(Boss)) return;

	// 중심 위치
	WaveCenter = Boss->GetActorLocation();
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
 
	CurrentWaveMaxRadius = FMath::Max(FirstWaveRadius + WaveRadiusOffset * CurrentWaveIndex, InitRingRadius);

	// 파장의 시작 위치는 거문고가 늘어선 원에서 출발한다.
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

	DrawWaveRing();
}

float UBossAttack_ResonantWave::GetWaveInnerEdge() const
{
	return FMath::Max(CurrentWaveRadius - WaveThickness, InitRingRadius);
}

void UBossAttack_ResonantWave::DrawWaveRing() const
{
#if ENABLE_DRAW_DEBUG
	const float InnerEdge = GetWaveInnerEdge();
	if (CurrentWaveRadius - InnerEdge <= 1.f) return;

	// 각 각도마다 안쪽·바깥쪽 점을 짝으로 넣는다.
	TArray<FVector> Vertices;
	Vertices.Reserve(WaveSegmentCount * 2);

	for (int32 Segment = 0; Segment < WaveSegmentCount; ++Segment)
	{
		const float Angle = 2.f * PI * Segment / WaveSegmentCount;
		const FVector Direction(FMath::Cos(Angle), FMath::Sin(Angle), 0.f);

		Vertices.Add(WaveCenter + Direction * InnerEdge);
		Vertices.Add(WaveCenter + Direction * CurrentWaveRadius);
	}

	// 네 점이 이루는 사각형을 삼각형 둘로 쪼개 띠를 채운다.
	TArray<int32> Indices;
	Indices.Reserve(WaveSegmentCount * 6);

	for (int32 Segment = 0; Segment < WaveSegmentCount; ++Segment)
	{
		const int32 NextSegment = (Segment + 1) % WaveSegmentCount;

		const int32 Inner = Segment * 2;
		const int32 Outer = Inner + 1;
		const int32 NextInner = NextSegment * 2;
		const int32 NextOuter = NextInner + 1;

		Indices.Add(Inner);
		Indices.Add(Outer);
		Indices.Add(NextOuter);

		Indices.Add(Inner);
		Indices.Add(NextOuter);
		Indices.Add(NextInner);
	}

	DrawDebugMesh(GetWorld(), Vertices, Indices, FColor::Red, false, WaveLifeTime);
#endif
}

void UBossAttack_ResonantWave::CheckWaveHit()
{
	if (bWaveHit) return;

	APawn* Player = GetTargetPlayer();
	if (!Player) return;
	if (!Player->Implements<UCombatInterface>()) return;

	const float Distance = FVector::Dist2D(Player->GetActorLocation(), WaveCenter);

	const float InnerEdge = GetWaveInnerEdge();

	if (Distance > CurrentWaveRadius) return;
	if (Distance < InnerEdge) return;

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
