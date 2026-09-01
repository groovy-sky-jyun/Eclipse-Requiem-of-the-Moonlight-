// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BossAttackBase.h"
#include "BossAttack_ResonantWave.generated.h"

/**
 * 거문고를 원형으로 세우고, 점점 빨라지는 음파 링을 확산시킨다.
 */
UCLASS(DisplayName = "Resonant Wave")
class ECLIPSE_API UBossAttack_ResonantWave : public UBossAttackBase
{
	GENERATED_BODY()

public:
	UBossAttack_ResonantWave();

protected:
	virtual void OnStartup() override;

	virtual void OnActive() override;

	virtual void OnTick(float DeltaTime) override;

	virtual void OnRecovery() override;

	virtual void OnCancel() override;

	virtual void OnFinish() override;

	/** 다음 파를 시작한다. 남은 파가 없으면 후딜로 넘어간다. */
	void StartNextWave();

	/** 확장 중인 링이 플레이어를 지나쳤는지 검사한다. */
	void CheckWaveHit();

protected:
	UPROPERTY(EditAnywhere, Category = "Settings|Combat|ResonantWave", meta = (ClampMin = "1"))
	int32 WaveCount = 3;

	/** 파 하나가 끝까지 퍼지는 데 걸리는 시간. 모든 파가 같다. */
	UPROPERTY(EditAnywhere, Category = "Settings|Combat|ResonantWave")
	float WaveDuration = 1.2f;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|ResonantWave")
	float WaveInterval = 0.15f;

	/** 거문고가 원을 이루는 반경. 음파는 보스가 아니라 여기서 시작한다. */
	UPROPERTY(EditAnywhere, Category = "Settings|Combat|ResonantWave")
	float InitRingRadius = 350.f;

	/** 첫 파가 닿는 거리. */
	UPROPERTY(EditAnywhere, Category = "Settings|Combat|ResonantWave")
	float FirstWaveRadius = 700.f;

	/** 파마다 넓어지는 거리. 같은 시간에 더 가야 하므로 뒤로 갈수록 빨라진다. */
	UPROPERTY(EditAnywhere, Category = "Settings|Combat|ResonantWave")
	float WaveRadiusStep = 400.f;

	/** 링의 두께. 이 폭 안에 들어오면 맞는다. */
	UPROPERTY(EditAnywhere, Category = "Settings|Combat|ResonantWave")
	float WaveThickness = 120.f;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|ResonantWave")
	float WaveDamage = 25.f;

	FVector WaveOrigin = FVector::ZeroVector;

	int32 CurrentWaveIndex = 0;

	/** 이번 파가 닿는 거리. */
	float CurrentWaveMaxRadius = 0.f;

	float CurrentWaveRadius = 0.f;

	float CurrentWaveSpeed = 0.f;

	/** 파 하나당 한 번만 맞는다. */
	bool bWaveHit = false;

	FTimerHandle ResonantWaveTimer;
};
