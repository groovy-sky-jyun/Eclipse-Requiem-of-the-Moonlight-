// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BossAttackBase.h"
#include "Templates/SubclassOf.h"
#include "BossAttack_BloodBolt.generated.h"

class AAttack_BloodBolt;
class UNiagaraSystem;

/**
 * 유도 탄환을 일정 간격으로 연사한다.
 */
UCLASS(DisplayName = "Blood Bolt")
class ECLIPSE_API UBossAttack_BloodBolt : public UBossAttackBase
{
	GENERATED_BODY()

protected:
	virtual void OnWindup() override;

	void BloodBolt_FireSingleBolt(); // 타이머 콜백

protected:
	UPROPERTY(EditAnywhere, Category = "Settings|Combat|BloodBolt")
	TSubclassOf<AAttack_BloodBolt> BloodBoltClass;

	UPROPERTY(EditAnywhere, Category = "Settings|VFX")
	TObjectPtr<UNiagaraSystem> NS_BloodBoltTrail;

	//UPROPERTY(EditAnywhere, Category = "Settings|VFX")
	//TObjectPtr<UNiagaraSystem> NS_BloodBoltImpact; //명중 시 폭발

	FTimerHandle BloodBoltTimerHandle;

	int32 BloodBoltRemaining = 0; // 남은 Bolt 추적
};
