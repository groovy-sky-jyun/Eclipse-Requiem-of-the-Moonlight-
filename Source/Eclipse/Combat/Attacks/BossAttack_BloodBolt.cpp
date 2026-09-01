// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAttack_BloodBolt.h"
#include "EnemyBoss.h"
#include "Attack_BloodBolt.h"
#include "Engine/World.h"

void UBossAttack_BloodBolt::OnWindup()
{
	AEnemyBoss* Boss = GetBoss();
	if (!BloodBoltClass) return;

	APawn* Player = GetTargetPlayer();
	if (!Player) return;

	int32 BoltCount = (Boss->GetCurrentPhase() == 1) ? 3 : 5;
	float BoltSpeed = (Boss->GetCurrentPhase() == 3) ? 2200.f : 1800.f;

	// 발사 간격
	float FireInterval = 0.35f;

	BloodBoltRemaining = BoltCount;

	// 첫 발이 즉시 나가므로 여기서 바로 판정 구간이다.
	EnterActive();
	BloodBolt_FireSingleBolt();

	SetAttackTimer(
		BloodBoltTimerHandle,
		FTimerDelegate::CreateUObject(this, &UBossAttack_BloodBolt::BloodBolt_FireSingleBolt),
		FireInterval,
		true
	);
}

void UBossAttack_BloodBolt::BloodBolt_FireSingleBolt()
{
	AEnemyBoss* Boss = GetBoss();
	if (!IsValid(Boss)) return;

	BloodBoltRemaining--;
	const bool bLastBolt = (BloodBoltRemaining <= 0);

	// 탄을 먼저 쏘고 나서 끝났다고 알린다.
	// 순서가 반대면 BT가 다음 태스크로 넘어간 뒤에 마지막 탄이 발사된다.
	APawn* Player = GetTargetPlayer();
	if (Player && BloodBoltClass)
	{
		FVector SpawnLoc = Boss->GetActorLocation() + Boss->GetActorForwardVector() * 100.f + FVector(0.f, 0.f, -50.f);
		FVector Direction = (Player->GetActorLocation() - SpawnLoc).GetSafeNormal();
		FRotator SpawnRot = Direction.Rotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = Boss;
		SpawnParams.Instigator = Boss->GetInstigator();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AAttack_BloodBolt* Bolt = GetWorld()->SpawnActor<AAttack_BloodBolt>(BloodBoltClass, SpawnLoc, SpawnRot, SpawnParams);

		if (Bolt)
		{
			Bolt->SetDamage(30.f);
			Bolt->Launch(Direction);
		}
	}

	// 플레이어를 못 찾아 발사를 건너뛰었더라도 종료는 반드시 알린다.
	if (bLastBolt)
	{
		ClearAttackTimer(BloodBoltTimerHandle);
		EnterRecovery();
	}
}
