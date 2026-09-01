// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAttack_SilentSlash.h"
#include "Eclipse.h"
#include "EnemyBoss.h"
#include "SlashWave.h"
#include "Engine/World.h"

UBossAttack_SilentSlash::UBossAttack_SilentSlash()
{
	// 연주 0.6 + 정적 0.5
	WindupTime = 1.1f;
	RecoveryTime = 1.4f;
	MaxDuration = 5.f;
}

void UBossAttack_SilentSlash::OnWindup()
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
			SlashInterval * Index,
			false
		);
	}

	SetAttackTimer(
		SilentSlashTimer,
		FTimerDelegate::CreateWeakLambda(this, [this]() { EnterRecovery(); }),
		SlashInterval * SlashCount,
		false
	);
}

void UBossAttack_SilentSlash::FireSlash(int32 Index)
{
	AEnemyBoss* Boss = GetBoss();
	if (!IsValid(Boss)) return;

	APawn* Player = GetTargetPlayer();
	if (!Player) return;

	// 타마다 다시 조준한다. 제자리에 서 있으면 세 발을 모두 맞는다.
	FVector ToPlayer = Player->GetActorLocation() - Boss->GetActorLocation();
	ToPlayer.Z = 0.f;

	const FVector Direction = ToPlayer.GetSafeNormal();
	if (Direction.IsNearlyZero()) return;

	Boss->SetActorRotation(Direction.Rotation());

	const FVector SpawnLocation = Boss->GetActorLocation() + Direction * SlashSpawnOffset;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Boss;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ASlashWave* Wave = GetWorld()->SpawnActor<ASlashWave>(
		SlashWaveClass, SpawnLocation, Direction.Rotation(), SpawnParams);

	if (!Wave) return;

	const bool bFinalSlash = (Index == SlashCount - 1);
	Wave->Launch(Direction, bFinalSlash ? FinalSlashDamage : SlashDamage, Boss);

	ActiveWaves.Add(Wave);

	UE_LOG(LogEclipse, Log, TEXT("[SilentSlash] Slash %d/%d"), Index + 1, SlashCount);
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
