// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BossAttackBase.h"
#include "Templates/SubclassOf.h"
#include "BossAttack_ShadowCrash.generated.h"

class AAttack_Marker;
class UNiagaraSystem;

/**
 * 상승 후 표적 위치로 급강하해, 착지 지점에 원형 피해를 준다.
 */
UCLASS(DisplayName = "Shadow Crash")
class ECLIPSE_API UBossAttack_ShadowCrash : public UBossAttackBase
{
	GENERATED_BODY()

protected:
	virtual void OnStartup() override;

	void ShadowCrash_StartAscend();
	void ShadowCrash_StartTelegraph();
	void ShadowCrash_StartDive();
	void ShadowCrash_OnImpact();

protected:
	UPROPERTY(EditAnywhere, Category = "Settings|Combat")
	TSubclassOf<AAttack_Marker> AttackMarkerClass;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|ShadowCrash")
	float ShadowCrashMarkerRadius = 250.f;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|ShadowCrash")
	float ShadowCrashDamage = 100.f;

	UPROPERTY(EditAnywhere, Category = "Settings|VFX")
	TObjectPtr<UNiagaraSystem> NS_ShadowCrashLand; //착지 충격(땅 갈라짐)

	FVector ShadowCrashTargetLoc = FVector::ZeroVector;
	FVector ShadowCrashOriginLoc = FVector::ZeroVector;
	FTimerHandle ShadowCrashTimer;
};
