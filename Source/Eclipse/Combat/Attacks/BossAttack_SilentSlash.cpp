// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAttack_SilentSlash.h"
#include "Eclipse.h"
#include "EnemyBoss.h"
#include "SlashWave.h"
#include "Engine/World.h"

void UBossAttack_SilentSlash::OnStart()
{
	if (!SlashWaveClass)
	{
		UE_LOG(LogEclipse, Error, TEXT("[SilentSlash] SlashWaveClass is not set"));
		Finish();
		return;
	}

	ActiveWaves.Reset();

	// 프로토타입 단계라 예열 없이 참격 연타만 검증한다.
	SetAttackState(EBossAttackState::Active);

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
		FTimerDelegate::CreateWeakLambda(this, [this]()
		{
			SetAttackState(EBossAttackState::Recovery);
			Finish();
		}),
		SlashInterval * (SlashCount - 1) + RecoveryTime,
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
