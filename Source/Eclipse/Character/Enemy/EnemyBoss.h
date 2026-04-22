// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "Templates/SubclassOf.h"
#include "EnemyBoss.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API AEnemyBoss : public AEnemyBase
{
	GENERATED_BODY()
	

public:
	AEnemyBoss();


protected:
	virtual void BeginPlay() override;
	virtual void HandleTakeDamage_Implementation(float DamageAmount, AActor* Attacker) override;

public:
	// 보스 페이즈 관리용 변수 (1페이즈 지상, 2페이즈 비행 등)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Phase")
	int32 CurrentPhase;

	// 체력 비율에 따라 페이즈를 전환하는 함수
	void CheckPhaseTransition();


	UPROPERTY(EditAnywhere, Category = "Combat|Boss")
	TSubclassOf<class AEnemyMinion> MinionClass;

	UFUNCTION(BlueprintCallable, Category = "Combat|Boss")
	void SpawnMinions(int32 Amount);
};
