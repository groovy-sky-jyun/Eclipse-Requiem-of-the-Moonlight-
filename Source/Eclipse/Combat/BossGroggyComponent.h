// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BossGroggyComponent.generated.h"

class AEnemyBoss;
class UAnimMontage;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStaggerChanged, float /*Current*/, float /*Max*/);

USTRUCT(BlueprintType)
struct FGroggyPhaseSettings
{
	GENERATED_BODY()

	// 이 값만큼 스태거가 쌓이면 그로기에 진입한다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0"))
	float StaggerThreshold = 100.f;

	// 그로기 유지 시간(초). 기상 모션은 포함하지 않는다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0"))
	float GroggyDuration = 4.f;
};

/**
 * 보스의 스태거 누적과 그로기 진입 / 유지 / 기상 담당
 */
UCLASS(ClassGroup = (Boss), meta = (BlueprintSpawnableComponent))
class ECLIPSE_API UBossGroggyComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBossGroggyComponent();

	void AddStagger(int32 CurrentPhase, float StaggerValue);

	float GetCurrentStagger() const { return CurrentStagger; }

	/** 현재 페이즈의 스태거 임계값. 설정이 없으면 0. */
	float GetStaggerThreshold() const;

	// 게이지가 바뀔 때마다 방송한다. (누적, 감쇠, 그로기 종료)
	FOnStaggerChanged OnStaggerChangedDelegate;


protected:
	virtual void BeginPlay() override;

	/** 스태거 감쇠 중에만 켜진다. */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** 인덱스 0이 페이즈 1이다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Groggy")
	TArray<FGroggyPhaseSettings> PhaseSettings;

	// Enable Auto Blend Out을 켜야 종료 델리게이트가 호출된다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Groggy")
	TObjectPtr<UAnimMontage> GetUpMontage;

	UPROPERTY(Transient)
	TObjectPtr<AEnemyBoss> Boss;

	
private:
	/** 그로기 진입. 스태거가 임계값에 도달하면 호출된다. */
	void EnterGroggy();

	// 진입 모션이 끝나면 호출. GroggyDuration 동안 유지한다.
	void HoldGroggy();

	// 유지 시간이 끝나면 호출. 기상 모션을 재생한다.
	void GetUp();

	void HandleGetUpMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// 기상 모션이 끝나면 호출. 그로기를 해제하고 면역을 시작한다.
	void FinishGroggy();

	void StartStaggerImmunity();

	// 마지막 적중 후 StaggerDecayDelay가 지나면 호출. 게이지를 서서히 줄인다.
	void StartStaggerDecay();
	void StopStaggerDecay();

	// 멤버 값과 블랙보드 bIsGroggy를 함께 바꾼다.
	void SetGroggy(bool bNewGroggy);

	// 게이지 값을 바꾸고 OnStaggerChangedDelegate를 방송한다.
	void SetCurrentStagger(float NewStagger);

	bool IsBossAlive() const;
	bool IsStaggerImmune() const;

	float CurrentStagger = 0.f;
	bool bIsGroggy = false;

	// 기상 직후 스태거가 바로 쌓이지 않도록 막는 시간(초)
	UPROPERTY(EditAnywhere, Category = "Settings|Groggy", meta = (ClampMin = "0.0"))
	float StaggerImmunityDuration = 2.f;

	// 마지막 적중 후 이 시간(초)이 지나면 게이지가 줄어들기 시작한다.
	UPROPERTY(EditAnywhere, Category = "Settings|Groggy", meta = (ClampMin = "0.0"))
	float StaggerDecayDelay = 5.f;

	// 초당 줄어드는 스태거 양
	UPROPERTY(EditAnywhere, Category = "Settings|Groggy", meta = (ClampMin = "0.0"))
	float StaggerDecayPerSecond = 10.f;

	FTimerHandle StaggerImmunityHandle;
	FTimerHandle GroggyHoldHandle;
	FTimerHandle StaggerDecayDelayHandle;


};
