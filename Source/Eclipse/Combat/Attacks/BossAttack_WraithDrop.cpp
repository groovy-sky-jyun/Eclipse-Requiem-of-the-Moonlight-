// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAttack_WraithDrop.h"
#include "Eclipse.h"
#include "EnemyBoss.h"
#include "EnemyMinion.h"
#include "BossAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/World.h"

void UBossAttack_WraithDrop::OnWindup()
{
	AEnemyBoss* Boss = GetBoss();

	if (!MinionClass || !SpawnEQS)
	{
		UE_LOG(LogEclipse, Error, TEXT("[Boss] MinionClass or SpawnEQS is NULL"));
		Finish();
		return;
	}

	if (Boss->GetActiveWraithCount() > 0) return;

	// EQS 실행 
	FEnvQueryRequest QueryRequest(SpawnEQS, Boss);
	QueryRequest.Execute(EEnvQueryRunMode::AllMatching, this, &UBossAttack_WraithDrop::OnSpawnEQSFinished);
}

void UBossAttack_WraithDrop::OnSpawnEQSFinished(TSharedPtr<FEnvQueryResult> Result)
{
	AEnemyBoss* Boss = GetBoss();
	if (!IsValid(Boss)) return;

	if (!Result.IsValid() || !Result->IsSuccessful())
	{
		UE_LOG(LogEclipse, Warning, TEXT("[Boss] WraithDrop : Can't found Spawn Location"));
		Finish();
		return;
	}

	//EQS로 부터 좌표 받기
	TArray<FVector> AllLocations;
	Result->GetAllAsLocations(AllLocations);

	int32 MaxSpawnCount = (Boss->GetCurrentPhase() >= 3) ? 4 : 2;
	TArray<FVector> FinalSpawnLocations;

	for (int32 i = 0; i < FMath::Min(MaxSpawnCount, AllLocations.Num()); i++)
	{
		FinalSpawnLocations.Add(AllLocations[i]);
		
		// 바닥에 경고용 안개 이펙트 스폰
		if(NS_WraithSummon)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NS_WraithSummon, AllLocations[i]);
		}
	}

	// SpawnDelayTime's 뒤에 실제 Wraith 액터 스폰 (이펙트 지연시간)
	FTimerDelegate TimerDel = FTimerDelegate::CreateUObject(this, &UBossAttack_WraithDrop::SpawnWraithsFromFog, FinalSpawnLocations);
	SetAttackTimer(WraithSpawnTimerHandle, TimerDel, SpawnDelayTime, false);
}

void UBossAttack_WraithDrop::SpawnWraithsFromFog(TArray<FVector> SpawnLocations)
{
	// 소환 자체에는 판정이 없지만, 끊기면 안 되는 구간이라 Active로 둔다.
	EnterActive();

	AEnemyBoss* Boss = GetBoss();
	if (!IsValid(Boss)) return;

	UWorld* World = GetWorld();
	if (!World) return;
	
	APawn* Player = GetTargetPlayer();
	if (!IsValid(Player)) return;

	for (const FVector& SpawnLoc : SpawnLocations)
	{
		FRotator SpawnRot = FRotator::ZeroRotator;
		FVector Direction = (Player->GetActorLocation() - SpawnLoc).GetSafeNormal();
		SpawnRot = Direction.Rotation();
		SpawnRot.Pitch = 0.f;
		SpawnRot.Roll = 0.f;

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = Boss;
		SpawnParams.Instigator = Boss->GetInstigator();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AEnemyMinion* Minion = World->SpawnActor<AEnemyMinion>(MinionClass, SpawnLoc, SpawnRot, SpawnParams);

		if (Minion)
		{
			Boss->OnWraithSpawned(Minion);
		}
	}

	EnterRecovery();
}
