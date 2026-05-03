// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "Templates/SubclassOf.h"
#include "EnemyBoss.generated.h"

UENUM(BlueprintType)
enum class EBossPhase : uint8
{
	Phase01 UMETA(DisplayName = "Phase01"),
	Phase02 UMETA(DisplayName = "Phase02"),
	Phase03 UMETA(DisplayName = "Phase03")
};

UENUM(BlueprintType)
enum class EBossState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Attacking UMETA(DisplayName = "Attacking"),
	PhaseTransition UMETA(DisplayName = "PhaseTransition"),
	Ultimate_Executing UMETA(DisplayName = "UltimateExecuting")
};

UCLASS()
class ECLIPSE_API AEnemyBoss : public AEnemyBase
{
	GENERATED_BODY()
	

public:
	AEnemyBoss();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBossPhase BossPhase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBossState BossState;

	UFUNCTION(BlueprintCallable, Category = "Settings|Combat|Phase")
	int32 GetCurrentPhase() const { return CurrentPhase; }

protected:
	virtual void BeginPlay() override;
	virtual void HandleTakeDamage_Implementation(float DamageAmount, AActor* Attacker) override;

	// 보스 페이즈 관리용 변수 (1페이즈 지상, 2페이즈 비행 등)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Combat|Phase")
	int32 CurrentPhase = 1;

	void CheckPhaseTransition(); // 체력 비율에 따라 페이즈를 전환


public:
	UPROPERTY(EditAnywhere, Category = "Settings|Combat|Boss")
	TSubclassOf<class AEnemyMinion> MinionClass;

	UFUNCTION(BlueprintCallable, Category = "Settings|Combat|Boss")
	void SpawnMinions(int32 Amount);
};
