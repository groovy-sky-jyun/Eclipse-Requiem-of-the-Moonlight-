// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "BossAttack.h"
#include "BossAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Templates/SubclassOf.h"
#include "EnemyBoss.generated.h"

class AEnemyMinion;
class AAttack_BloodBolt;
class UBoxComponent;


UCLASS()
class ECLIPSE_API AEnemyBoss : public AEnemyBase
{
	GENERATED_BODY()
	

public:
	AEnemyBoss();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Controller")
	TObjectPtr<ABossAIController> AI;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Controller")
	TObjectPtr<UBlackboardComponent> BB;

	virtual void BeginPlay() override;
	virtual void HandleTakeDamage_Implementation(float DamageAmount, AActor* Attacker) override;
	virtual void Die_Implementation() override;


// ── 페이즈 ───────────────────────────────────────────────
public:
	UFUNCTION(BlueprintCallable, Category = "Boss|Phase")
	int32 GetCurrentPhase() const { return CurrentPhase; }

	UFUNCTION(BlueprintCallable, Category = "Boss|Phase")
	void EnterPhase(int32 NewPhase);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Phase")
	int32 CurrentPhase = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Phase")
	float Phase2Threshold = 0.6f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Phase")
	float Phase3Threshold = 0.3f;


// ── 비행 ─────────────────────────────────────────────────
public:
	UFUNCTION(BlueprintCallable, Category = "Boss|Movement")
	void SetFlying(bool bFly);

	UFUNCTION(BlueprintPure, Category = "Boss|Movement")
	bool IsFlying() const { return bIsFlying; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Movement")
	bool bIsFlying = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Movement")
	float FlyHeight = 400.f;


// ── 공격 영역 ─────────────────────────────────────────────
public:
	UFUNCTION(BlueprintCallable, Category = "Boss|Combat")
	void ExecuteAttack(EBossAttackType Attack);

	UFUNCTION(BlueprintCallable, Category = "Boss|Combat")
	void SetInvincible(bool bInvincible);

	TMap<EBossAttackType, float> AttackLastUsedList;

protected:
	void Attack_BloodBolt();
	void Attack_ShadowCrash();
	void Attack_WraithDrop();
	void Attack_DarkSweep();
	void Attack_LunarBeam();
	void Defense_EclipseVeil();

	UPROPERTY(EditAnywhere, Category = "Settings|Combat")
	TSubclassOf<AAttack_BloodBolt> BloodBoltClass;

	FTimerHandle BloodBoltTimerHandle;

	// 남은 Bolt 추적
	int32 BloodBoltRemaining = 0;

	// 타이머 콜백
	void FireSingleBolt();

	UPROPERTY(EditAnywhere, Category = "Settings|Combat")
	TSubclassOf<AEnemyMinion> MinionClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Combat")
	int32 ActiveWraithCount = 0;

	bool bEclipseVeilUsed = false;

public:
	UFUNCTION(BlueprintCallable, Category = "Boss|Combat")
	void OnWraithDied();


// ── Stagger ─────────────────────────────────────────────
protected:
	// 누적된 스태거용 데미지
	float StaggerAccumulated = 0.f;

	// 스태거 발동 누적 데미지 조건
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Stagger")
	float StaggerThreshold = 300.f;

	// 마지막 피격으로부터 이 시간(초)이 지나면 누적 데미지 초기화
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Stagger")
	float StaggerResetTime = 5.f;

	// 마지막 피격 시각 기록용
	float TimeSinceLastHit = 0.f;

	// 페이즈 전환 시 호출 (페이즈별 데미지 조건 변경)
	void UpdateStaggerThresholdByPhase();
};