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

protected:
	virtual void OnStart() override;

	virtual void OnTick(float DeltaTime) override;

	virtual void OnCancel() override;

	virtual void OnFinish() override;
};
