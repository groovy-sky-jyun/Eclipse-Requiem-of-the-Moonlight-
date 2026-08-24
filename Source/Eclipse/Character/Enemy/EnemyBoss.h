// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "BossAttack.h"
#include "Templates/SubclassOf.h"
#include "EnemyBoss.generated.h"

class AEnemyMinion;
class UBossAttackBase;
class UBossAttackComponent;
class UBossPhaseComponent;

UCLASS()
class ECLIPSE_API AEnemyBoss : public AEnemyBase
{
	GENERATED_BODY()
	
	// 페이즈 전환 시 보스의 1회성 플래그를 직접 만진다.
	friend class UBossPhaseComponent;

public:
	AEnemyBoss();

protected:


	virtual void BeginPlay() override;
	virtual void OnDamaged(float DamageAmount, AActor* Attacker, bool bLethal) override;
	virtual void HandleDeath() override;


// ── 페이즈 ───────────────────────────────────────────────
// 판정과 상태는 UBossPhaseComponent가 가진다. 여기 있는 건 통로일 뿐이다.
public:
	UFUNCTION(BlueprintCallable, Category = "Phase")
	int32 GetCurrentPhase() const;

	UFUNCTION(BlueprintCallable, Category = "Phase")
	void EnterPhase(int32 NewPhase);

	UFUNCTION(BlueprintPure, Category = "Phase")
	UBossPhaseComponent* GetPhaseComponent() const { return PhaseComponent; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Phase")
	TObjectPtr<UBossPhaseComponent> PhaseComponent;


// ── 공격 영역 ─────────────────────────────────────────────
// 선택과 실행은 UBossAttackComponent가, 개별 공격의 내용은 UBossAttackBase 파생 클래스가 가진다.
public:
	UFUNCTION(BlueprintPure, Category = "Combat")
	UBossAttackComponent* GetAttackComponent() const { return AttackComponent; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Combat")
	TObjectPtr<UBossAttackComponent> AttackComponent;


// ── 망령 카운트 ───────────────────────────────────────────
// 소환 연출은 UBossAttack_WraithDrop이 하고, 생존 수만 보스가 센다.
public:
	UFUNCTION(BlueprintPure, Category = "Combat|WraithDrop")
	int32 GetActiveWraithCount() const { return ActiveWraithCount; }

	void OnWraithSpawned(AEnemyMinion* Minion);

	UFUNCTION(BlueprintCallable, Category = "Combat|WraithDrop")
	void OnWraithDied();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Combat|WraithDrop")
	int32 ActiveWraithCount = 0;

	bool bEclipseVeilUsed = false;
};
