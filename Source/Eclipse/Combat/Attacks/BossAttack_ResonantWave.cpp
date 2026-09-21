// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAttack_ResonantWave.h"
#include "Eclipse.h"
#include "EnemyBoss.h"
#include "CombatInterface.h"
#include "Geomungo.h"
#include "Components/CapsuleComponent.h"
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
	AEnemyBoss* Boss = GetBoss();
	if (!IsValid(Boss)) return;

	// 거문고 원과 음파가 같은 중심 사용
	WaveCenter = Boss->GetActorLocation();

	PlaceGeomungoRing();
}

void UBossAttack_ResonantWave::PlaceGeomungoRing()
{
	AEnemyBoss* Boss = GetBoss();
	UWorld* World = GetWorld();
	if (!IsValid(Boss) || !World) return;

	AGeomungo* HeldGeomungo = Boss->GetGeomungo();
	if (!IsValid(HeldGeomungo))
	{
		UE_LOG(LogEclipse, Warning, TEXT("[ResonantWave] Boss has no Geomungo"));
		return;
	}

	// 손에 든 거문고는 그대로 붙여둔 채 숨긴다. 취소되면 다시 보이기만 하면 된다.
	HeldGeomungo->SetActorHiddenInGame(true);

	const float FloorZ = WaveCenter.Z - Boss->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	RingHeightZ = FloorZ + GeomungoHeight;
	RingSpinAngle = 0.f;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Boss;
	SpawnParams.Instigator = Boss;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// 배열 위치가 곧 각도 인덱스다. 스폰이 실패해도 자리를 비워 둔다.
	RingGeomungos.SetNum(GeomungoCount);

	int32 PlacedCount = 0;

	for (int32 Index = 0; Index < GeomungoCount; ++Index)
	{
		AGeomungo* Clone = World->SpawnActor<AGeomungo>(HeldGeomungo->GetClass(), GetGeomungoTransform(Index), SpawnParams);
		RingGeomungos[Index] = Clone;

		if (Clone)
		{
			++PlacedCount;
		}
	}

	UE_LOG(LogEclipse, Log, TEXT("[ResonantWave] Geomungo ring placed (%d/%d)"), PlacedCount, GeomungoCount);
}

FTransform UBossAttack_ResonantWave::GetGeomungoTransform(int32 Index) const
{
	// 등분 각도에 누적 회전각을 더해 원을 돌린다.
	const float Yaw = 360.f * Index / FMath::Max(GeomungoCount, 1) + RingSpinAngle;

	// 음파가 나가는 바깥쪽을 바라본다. Yaw 방향이 곧 중심에서 뻗어나가는 방향이다.
	const FRotator Rotation(0.f, Yaw, 0.f);
	const FVector Direction = Rotation.Vector();

	const FVector Location(
		WaveCenter.X + Direction.X * InitRingRadius,
		WaveCenter.Y + Direction.Y * InitRingRadius,
		RingHeightZ);

	return FTransform(Rotation, Location);
}

void UBossAttack_ResonantWave::UpdateGeomungoRing(float DeltaTime)
{
	if (RingGeomungos.IsEmpty()) return;

	// 각도가 무한히 커지지 않도록 한 바퀴 안으로 접는다.
	RingSpinAngle = FMath::Fmod(RingSpinAngle + GeomungoSpinSpeed * DeltaTime, 360.f);

	for (int32 Index = 0; Index < RingGeomungos.Num(); ++Index)
	{
		AGeomungo* Clone = RingGeomungos[Index];
		if (!IsValid(Clone)) continue;

		const FTransform Transform = GetGeomungoTransform(Index);
		Clone->SetActorLocationAndRotation(Transform.GetLocation(), Transform.GetRotation());
	}
}

void UBossAttack_ResonantWave::ClearGeomungoRing()
{
	for (AGeomungo* Clone : RingGeomungos)
	{
		if (IsValid(Clone))
		{
			Clone->Destroy();
		}
	}
	RingGeomungos.Reset();

	if (AEnemyBoss* Boss = GetBoss())
	{
		if (AGeomungo* HeldGeomungo = Boss->GetGeomungo())
		{
			HeldGeomungo->SetActorHiddenInGame(false);
		}
	}
}

void UBossAttack_ResonantWave::OnActive()
{
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
	// Active 가드보다 위에 둔다. 배치 연출(Startup)과 후딜에도 돌아야 한다.
	UpdateGeomungoRing(DeltaTime);

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

	ICombatInterface::Execute_TakeCombatDamage(Player, FCombatDamage(WaveDamage), GetBoss());
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
	// Cancel도 Finish를 거치므로 정리는 여기 한 곳에서만 한다.
	ClearGeomungoRing();
}
