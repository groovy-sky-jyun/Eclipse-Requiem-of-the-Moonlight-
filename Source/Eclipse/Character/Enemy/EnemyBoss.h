// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "BossAttack.h"
#include "BossAIController.h"
#include "Templates/SubclassOf.h"
#include "EnemyBoss.generated.h"

class AEnemyMinion;
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


// ── 감지 영역 ─────────────────────────────────────────────
public:
	UFUNCTION()
	void OnAggroOverlap(UPrimitiveComponent* Overlapped, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Detection")
	TObjectPtr<UBoxComponent> AggroBox;


// ── 공격 영역 ─────────────────────────────────────────────
public:
	UFUNCTION(BlueprintCallable, Category = "Boss|Combat")
	void ExecuteAttack(EBossAttackType Attack);

	TMap<EBossAttackType, float> AttackLastUsedTime;

protected:
	void Attack_BloodBolt();
	void Attack_WraithDrop();
	void Attack_LunarBeam();
	void Attack_DamningTether();
	void Defense_MiasmaStep();
	void Defense_EclipseVeil();

	UPROPERTY(EditAnywhere, Category = "Settings|Combat")
	TSubclassOf<AEnemyMinion> MinionClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Combat")
	int32 ActiveWraithCount = 0;

	bool bEclipseVeilUsed = false;

public:
	UFUNCTION(BlueprintCallable, Category = "Boss|Combat")
	void SetInvincible(bool bInvincible);

	UFUNCTION(BlueprintCallable, Category = "Boss|Combat")
	void OnWraithDied();
};
