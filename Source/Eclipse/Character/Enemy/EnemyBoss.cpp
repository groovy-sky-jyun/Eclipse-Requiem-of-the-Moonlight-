// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBoss.h"
#include "EnemyMinion.h"

AEnemyBoss::AEnemyBoss()
{
	// 보스다운 압도적인 스탯 초기화
	MaxHealth = 1500.f;
	CurrentHealth = MaxHealth;

	// 시작은 무조건 1페이즈
	CurrentPhase = 1;
}

void AEnemyBoss::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemyBoss::HandleTakeDamage_Implementation(float DamageAmount, AActor* Attacker)
{
	Super::HandleTakeDamage_Implementation(DamageAmount, Attacker);

	if (!IsDead_Implementation())
	{
		CheckPhaseTransition();
	}
}

void AEnemyBoss::CheckPhaseTransition()
{
	// 예: 체력이 50% 이하로 떨어졌고, 아직 1페이즈라면 2페이즈로 돌입
	if (CurrentPhase == 1 && CurrentHealth <= (MaxHealth * 0.5f))
	{
		CurrentPhase = 2;

		SpawnMinions(6);

		UE_LOG(LogTemp, Warning, TEXT("=== BOSS PHASE 2 START! ==="));

		// 달을 등지고 공중으로 날아오르는 몽타주 재생, EQS 쿼리 갱신, 
		// 투사체 패턴 변경 등의 특수 로직
	}
}

void AEnemyBoss::SpawnMinions(int32 Amount)
{
	if (!MinionClass) return;

	UWorld* World = GetWorld();
	if (World)
	{
		for (int32 i = 0; i < Amount; i++)
		{
			// 보스 위치에서 약간 떨어진 랜덤 위치 계산
			FVector SpawnLocation = GetActorLocation() + FVector(FMath::RandRange(-200, 200), FMath::RandRange(-200, 200), 0);
			FRotator SpawnRotation = GetActorRotation();

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			// 실제로 월드에 스폰!
			AEnemyMinion* SpawnedMinion = World->SpawnActor<AEnemyMinion>(MinionClass, SpawnLocation, SpawnRotation, SpawnParams);

			if (SpawnedMinion)
			{
				UE_LOG(LogTemp, Warning, TEXT("Minion Spawned!"));
			}
		}
	}
}
