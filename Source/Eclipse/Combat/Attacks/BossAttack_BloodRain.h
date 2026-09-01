// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BossAttackBase.h"
#include "BossAttack_BloodRain.generated.h"

/**
 * 폭주 후 지면 균열과 공중 낙하로 이어지는 2단 광역 궁극기.
 */
UCLASS(DisplayName = "Blood Rain")
class ECLIPSE_API UBossAttack_BloodRain : public UBossAttackBase
{
	GENERATED_BODY()

protected:
	virtual void OnStartup() override;

	virtual void OnTick(float DeltaTime) override;

	virtual void OnCancel() override;

	virtual void OnFinish() override;
};
