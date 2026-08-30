// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BossAttackBase.h"   // EBossAttackState
#include "BossPhaseComponent.generated.h"

class AEnemyBoss;

USTRUCT(BlueprintType)
struct FBossPhaseData
{
	GENERATED_BODY()

	// 이 페이즈에 진입하는 HP 비율. 배열은 내림차순으로 넣는다. 페이즈 1은 1.0.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float EnterHealthRatio = 1.0f;

	// 이 페이즈의 스태거 발동 누적 데미지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0"))
	float StaggerThreshold = 100.f;

};

/**
 * 보스의 페이즈 전환과 스태거 누적 담당
 *
 * 페이즈 판정은 폴링하지 않는다. ABaseCharacter::OnHealthChangedDelegate에 구독해
 * HP가 바뀌는 순간에만 조건을 다시 본다.
 */
UCLASS(ClassGroup = (Boss), meta = (BlueprintSpawnableComponent))
class ECLIPSE_API UBossPhaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBossPhaseComponent();

protected:
	virtual void BeginPlay() override;

// ── Phase ─────────────────────────────────────────────
public:
	UFUNCTION(BlueprintCallable, Category = "Phase")
	int32 GetCurrentPhase() const { return CurrentPhase; }

	UFUNCTION(BlueprintPure, Category = "Phase")
	int32 GetPhaseCount() const { return PhaseDataTable.Num(); }

	UFUNCTION(BlueprintCallable, Category = "Phase")
	void EnterPhase(int32 NewPhase);


protected:
	/** OnHealthChangedDelegate에 묶인다. HP가 바뀔 때마다 페이즈 조건을 다시 본다. */
	UFUNCTION()
	void HandleHealthChanged(float Current, float Max);

	int32 FindPhase(float HealthRatio) const;

	const FBossPhaseData& GetCurrentPhaseData() const;

	/** 인덱스 0이 페이즈 1이다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Phase")
	TArray<FBossPhaseData> PhaseDataTable;

	UPROPERTY(Transient)
	TObjectPtr<AEnemyBoss> Boss;


private:
	int32 CurrentPhase = 1;


// ── Stagger ─────────────────────────────────────────────
public:
	/** 피격 데미지를 스태거 게이지에 누적한다. 임계값을 넘으면 그로기로 보낸다. */
	void AddStaggerDamage(float DamageAmount);

	/** 그로기 진입. 필살기 인터럽트 경로도 여기로 들어온다. */
	void TriggerGroggy();

protected:
	/** 공격 단계 구독. 예약된 그로기를 Recovery나 Idle에서 발동시킨다. */
	void HandleAttackStateChanged(EBossAttackState NewState);

	// 스태거가 찼지만 공격 중이라 발동을 미룬 상태
	bool bGroggyPending = false;

protected:
	// 누적된 스태거용 데미지
	float StaggerAccumulated = 0.f;

	// 마지막 피격으로부터 이 시간(초)이 지나면 누적 데미지 초기화
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Stagger")
	float StaggerResetTime = 5.f;

	// 마지막 피격 시각 기록용
	float TimeSinceLastHit = 0.f;
};
