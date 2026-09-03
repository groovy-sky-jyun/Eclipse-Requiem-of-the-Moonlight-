// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAttack_SilentSlash.h"
#include "Eclipse.h"
#include "EnemyBoss.h"
#include "SlashWave.h"
#include "Engine/World.h"

UBossAttack_SilentSlash::UBossAttack_SilentSlash()
{
	// 연주 0.6 + 정적 0.5
	StartupTime = 1.1f;
	RecoveryTime = 1.4f;
	MaxDuration = 5.f;
}

void UBossAttack_SilentSlash::OnStartup()
{
	if (!SlashWaveClass)
	{
		UE_LOG(LogEclipse, Error, TEXT("[SilentSlash] SlashWaveClass is not set"));
		Finish();
		return;
	}

	ActiveWaves.Reset();

	// 연주에서 정적으로. 텔레그래프는 여기에 붙는다.
}

void UBossAttack_SilentSlash::OnActive()
{
	FireSlash(0);

	for (int32 Index = 1; Index < SlashCount; ++Index)
	{
		FTimerHandle SlashTimer;
		SetAttackTimer(
			SlashTimer,
			FTimerDelegate::CreateWeakLambda(this, [this, Index]() { FireSlash(Index); }),
			GetSlashDelay(Index),
			false
		);
	}

	// 마지막 타보다 한 간격 뒤에 후딜로 넘어간다.
	SetAttackTimer(
		SilentSlashTimer,
		FTimerDelegate::CreateWeakLambda(this, [this]() { EnterRecovery(); }),
		GetSlashDelay(SlashCount - 1) + SlashInterval,
		false
	);
}

float UBossAttack_SilentSlash::GetSlashDelay(int32 Index) const
{
	if (Index <= 0) return 0.f;

	// 마지막 타 앞만 벌린다. 익힌 리듬보다 반 박자 늦게 와야 판단할 틈이 생긴다.
	const bool bFinalSlash = (Index == SlashCount - 1);

	return SlashInterval * (Index - 1) + (bFinalSlash ? FinalSlashDelay : SlashInterval);
}

void UBossAttack_SilentSlash::FireSlash(int32 Index)
{
	AEnemyBoss* Boss = GetBoss();
	if (!IsValid(Boss)) return;

	APawn* Player = GetTargetPlayer();
	if (!Player) return;

	const bool bFinalSlash = (Index == SlashCount - 1);

	// 타마다 다시 조준한다. 마지막 타만 플레이어가 갈 자리를 노린다.
	const FVector AimLocation = bFinalSlash ? PredictAimLocation(Player) : Player->GetActorLocation();

	FVector ToAim = AimLocation - Boss->GetActorLocation();
	ToAim.Z = 0.f;

	const FVector Direction = ToAim.GetSafeNormal();
	if (Direction.IsNearlyZero()) return;

	Boss->SetActorRotation(Direction.Rotation());

	const FVector SpawnLocation = Boss->GetActorLocation() + Direction * SlashSpawnOffset;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Boss;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ASlashWave* Wave = GetWorld()->SpawnActor<ASlashWave>(
		SlashWaveClass, SpawnLocation, Direction.Rotation(), SpawnParams);

	if (!Wave) return;

	Wave->Launch(Direction, bFinalSlash ? FinalSlashDamage : SlashDamage, Boss);

	ActiveWaves.Add(Wave);

	UE_LOG(LogEclipse, Log, TEXT("[SilentSlash] Slash %d/%d"), Index + 1, SlashCount);
}

FVector UBossAttack_SilentSlash::PredictAimLocation(const APawn* Player) const
{
	const FVector PlayerLocation = Player->GetActorLocation();

	AEnemyBoss* Boss = GetBoss();
	if (!IsValid(Boss) || !SlashWaveClass) return PlayerLocation;

	// 탄속은 참격이 들고 있다. 값을 복사해두면 한쪽만 바뀌어 어긋난다.
	const ASlashWave* WaveDefault = SlashWaveClass->GetDefaultObject<ASlashWave>();
	const float SlashSpeed = WaveDefault ? WaveDefault->GetSlashSpeed() : 0.f;
	if (SlashSpeed <= 0.f) return PlayerLocation;

	FVector PlayerVelocity = Player->GetVelocity();
	PlayerVelocity.Z = 0.f;

	// 참격이 도착하는데 걸리는 시간 (거리 / 탄속)
	const float TravelTime = FVector::Dist2D(PlayerLocation, Boss->GetActorLocation()) / SlashSpeed;

	// 다음 위치 = 현재 위치 + (속도 * 시간) -> LeadFactor로 정확도 조절
	return PlayerLocation + PlayerVelocity * TravelTime * LeadFactor;
}

void UBossAttack_SilentSlash::OnRecovery()
{
}

void UBossAttack_SilentSlash::OnCancel()
{
	// 취소되면 아직 날아가는 참격도 같이 걷는다.
	for (ASlashWave* Wave : ActiveWaves)
	{
		if (IsValid(Wave))
		{
			Wave->Destroy();
		}
	}
	ActiveWaves.Reset();
}

void UBossAttack_SilentSlash::OnFinish()
{
	// 정상 종료면 이미 나간 참격은 끝까지 날아간다.
	ActiveWaves.Reset();
}
