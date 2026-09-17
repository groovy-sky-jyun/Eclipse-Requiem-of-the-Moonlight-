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
class UAnimMontage;
class AGeomungo;

UCLASS()
class ECLIPSE_API AEnemyBoss : public AEnemyBase
{
	GENERATED_BODY()
	
public:
	AEnemyBoss();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OnDamaged(float DamageAmount, AActor* Attacker, bool bLethal) override;
	virtual void OnDeath() override;


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

	/** 필살기 적중 시 호출한다. 예열 중이면 공격을 끊고 그로기로 보낸다. */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool TryGroggyByUltimate();

	UAnimMontage* GetRecoveryMontage() const { return RecoveryMontage; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Combat")
	TObjectPtr<UBossAttackComponent> AttackComponent;

	// 임시 : 공격별 몽타주가 생기기 전까지 모든 공격이 공유하는 후딜 모션
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Combat")
	TObjectPtr<UAnimMontage> RecoveryMontage;


// ── 거문고 ────────────────────────────────────────────────
public:
	UFUNCTION(BlueprintPure, Category = "Combat|Geomungo")
	AGeomungo* GetGeomungo() const { return Geomungo; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Combat|Geomungo")
	TSubclassOf<AGeomungo> GeomungoClass;

	// 보스 메시에 이 이름의 소켓이 있어야 한다.
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Combat|Geomungo")
	FName GeomungoSocketName = TEXT("GeomungoSocket");

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Settings|Combat|Geomungo")
	TObjectPtr<AGeomungo> Geomungo;

private:
	void SpawnGeomungo();


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
};
