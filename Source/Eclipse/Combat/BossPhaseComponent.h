// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BossPhaseComponent.generated.h"

class AEnemyBoss;

/**
 * 보스의 페이즈 전환과 스태거 누적을 담당한다.
 *
 * 페이즈가 바뀌는 원인은 HP와 피격 누적이므로 공격 선택과는 무관하다.
 * 공격 쪽(UBossAttackComponent)은 이 컴포넌트에게 현재 페이즈를 묻기만 한다.
 */
UCLASS(ClassGroup = (Boss), meta = (BlueprintSpawnableComponent))
class ECLIPSE_API UBossPhaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBossPhaseComponent();

protected:
	virtual void BeginPlay() override;

	/** 소유 보스. BeginPlay에서 한 번만 캐시한다. */
	UPROPERTY(Transient)
	TObjectPtr<AEnemyBoss> Boss;


// ── 페이즈 ───────────────────────────────────────────────
public:
	UFUNCTION(BlueprintCallable, Category = "Phase")
	int32 GetCurrentPhase() const { return CurrentPhase; }

	UFUNCTION(BlueprintCallable, Category = "Phase")
	void EnterPhase(int32 NewPhase);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Phase")
	int32 CurrentPhase = 1;

	// 페이즈 전환 HP 비율은 BTService_UpdatePhase가 자기 값으로 판정한다.
	// 여기에 같은 값을 또 두면 둘 중 뭐가 진짜인지 알 수 없게 된다.


// ── Stagger ─────────────────────────────────────────────
public:
	/** 보스가 피격될 때마다 호출된다. 누적치가 조건을 넘으면 스태거를 발동시킨다. */
	void NotifyDamageTaken(float DamageAmount);

protected:
	// 누적된 스태거용 데미지
	float StaggerAccumulated = 0.f;

	// 스태거 발동 누적 데미지 조건
	/** 페이즈별 스태거 임계값. 인덱스 0이 페이즈 1이다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Stagger")
	TArray<float> StaggerThresholdByPhase = { 100.f, 200.f, 300.f };

	/** 현재 페이즈에 적용 중인 값. 위 배열에서 파생되므로 직접 편집하지 않는다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Stagger")
	float StaggerThreshold = 100.f;

	// 마지막 피격으로부터 이 시간(초)이 지나면 누적 데미지 초기화
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Stagger")
	float StaggerResetTime = 5.f;

	// 마지막 피격 시각 기록용
	float TimeSinceLastHit = 0.f;

	// 페이즈 전환 시 호출 (페이즈별 데미지 조건 변경)
	void UpdateStaggerThresholdByPhase();
};
