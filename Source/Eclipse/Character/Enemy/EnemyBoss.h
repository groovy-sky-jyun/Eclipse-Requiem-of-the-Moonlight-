// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "BossAttack.h"
#include "BossAIController.h"
#include "Attack_Marker.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Templates/SubclassOf.h"
#include "EnemyBoss.generated.h"

class AEnemyMinion;
class AAttack_BloodBolt;
class AAttack_Marker;
class ASlashBeam;
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
	UFUNCTION(BlueprintCallable, Category = "Phase")
	int32 GetCurrentPhase() const { return CurrentPhase; }

	UFUNCTION(BlueprintCallable, Category = "Phase")
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
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetFlying(bool bFly);

	UFUNCTION(BlueprintPure, Category = "Movement")
	bool IsFlying() const { return bIsFlying; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Movement")
	bool bIsFlying = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Movement")
	float FlyHeight = 400.f;


// ── 공격 영역 ─────────────────────────────────────────────
public:
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ExecuteAttack(EBossAttackType Attack);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetInvincible(bool bInvincible);

	TMap<EBossAttackType, float> AttackLastUsedList;

protected:
	void Attack_BloodBolt();
	void Attack_ShadowCrash();
	void Attack_WraithDrop();
	void Attack_DarkSweep();
	void Attack_LunarBeam();
	void Attack_EclipseVeil();

	// ───────── BloodBolt ─────────
	UPROPERTY(EditAnywhere, Category = "Settings|Combat|BloodBolt")
	TSubclassOf<AAttack_BloodBolt> BloodBoltClass;

	FTimerHandle BloodBoltTimerHandle;

	int32 BloodBoltRemaining = 0; // 남은 Bolt 추적

	void BloodBolt_FireSingleBolt(); // 타이머 콜백

	// ───────── DarkSweep ─────────
	FVector DarkSweepDirection = FVector::ZeroVector;
	FVector DarkSweepStartLoc = FVector::ZeroVector;
	FVector DarkSweepEndLoc = FVector::ZeroVector;
	FTimerHandle DarkSweepTimer;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|DarkSweep")
	float DarkSweepDistance = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|DarkSweep")
	float DarkSweepHeight = 100.f;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|DarkSweep")
	float DarkSweepHalfWidth = 100.f;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|DarkSweep")
	float DarkSweepSpeed = 2500.f;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|DarkSweep")
	float DarkSweepDamage = 35.f;

	void DarkSweep_StartTelegraph();
	void DarkSweep_StartDash();
	void DarkSweep_CheckHit(const FVector& CurrentStepLoc);
	void DarkSweep_End();

	bool bDarkSweepHit = false; //중복 Hit 방지


	// ───────── ShadowCrash ─────────
	UPROPERTY(EditAnywhere, Category = "Settings|Combat")
	TSubclassOf<AAttack_Marker> AttackMarkerClass;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|ShadowCrash")
	float ShadowCrashMarkerRadius = 250.f;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|ShadowCrash")
	float ShadowCrashDamage = 100.f;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|ShadowCrash")
	float ClawDamage = 60.f; //할퀴기 1회 데미지

	FVector ShadowCrashTargetLoc = FVector::ZeroVector;
	FVector ShadowCrashOriginLoc = FVector::ZeroVector;
	FTimerHandle ShadowCrashTimer;
	FTimerHandle ClawComboTimer;
	int32 ClawComboRemaining = 0;

	void ShadowCrash_StartAscend();
	void ShadowCrash_StartTelegraph();
	void ShadowCrash_StartDive();
	void ShadowCrash_OnImpact();
	void ShadowCrash_DoClawHit();   // 할퀴기 1회 콜백

	// ───────── WraithDrop ─────────
	UPROPERTY(EditAnywhere, Category = "Settings|Combat|WraithDrop")
	TSubclassOf<AEnemyMinion> MinionClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Combat|WraithDrop")
	int32 ActiveWraithCount = 0;

	bool bEclipseVeilUsed = false;

	UFUNCTION(BlueprintCallable, Category = "Combat|WraithDrop")
	void OnWraithDied();

	// ───────── LunarBeam ─────────
	UPROPERTY(EditAnywhere, Category = "Settings|Combat|LunarBeam")
	float LunarBeamRadius = 280.f;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|LunarBeam")
	float LunarBeamOffset = 900.f; //외곽 원까지의 거리

	UPROPERTY(EditAnywhere, Category="Settings|Combat|LunarBeam")
	float LunarBeamDamage = 50.f;

	FTimerHandle LunarBeamTimer;

	TArray<FVector> LunarBeamImpactLoc;

	void LunarBeam_SpawnMarkers();
	void LunarBeam_IntensifyMarkers();
	void LunarBeam_Impact();

	TArray<FVector> GetLunarBeamOffsets() const;

	// ───────── EclipseVeil ─────────
	UPROPERTY(EditAnywhere, Category="Settings|Combat|EclipseVeil")
	TSubclassOf<ASlashBeam> SlashBeamClass;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|EclipseVeil")
	float EclipseDamage = 40.f;

	FTimerHandle EclipseVeilTimer;
	int32 EclipseCurrentRound = 0;

	void EclipseVeil_StartFog();
	void EclipseVeil_ExecuteRound(int32 Round);
	void EclipseVeil_SpawnSlash(FVector Center, float AngleDeg, float Damage);
	void EclipseVeil_End();

	struct FSlashConfig 
	{ 
		FVector offset;
		float AngleDeg;
	};

	TArray<FSlashConfig> GetSlashConfigs(int32 Round) const;



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