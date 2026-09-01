// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BossAttackBase.h"
#include "Templates/SubclassOf.h"
#include "BossAttack_WraithDrop.generated.h"

class AEnemyMinion;
class UEnvQuery;
class UNiagaraSystem;
struct FEnvQueryResult;

/**
 * EQS로 스폰 지점을 찾아 안개와 함께 망령을 소환한다.
 */
UCLASS(DisplayName = "Wraith Drop")
class ECLIPSE_API UBossAttack_WraithDrop : public UBossAttackBase
{
	GENERATED_BODY()

protected:
	virtual void OnWindup() override;

	void OnSpawnEQSFinished(TSharedPtr<FEnvQueryResult> Result);

	void SpawnWraithsFromFog(TArray<FVector> SpawnLocations);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Combat|WraithDrop")
	TSubclassOf<AEnemyMinion> MinionClass;

	UPROPERTY(EditDefaultsOnly, Category = "Settings|Combat|WraithDrop")
	TObjectPtr<UEnvQuery> SpawnEQS;

	UPROPERTY(EditAnywhere, Category = "Settings|VFX")
	TObjectPtr<UNiagaraSystem> NS_WraithSummon; //망령 소환 연기

	float SpawnDelayTime = 1.5f;

	FTimerHandle WraithSpawnTimerHandle;
};
