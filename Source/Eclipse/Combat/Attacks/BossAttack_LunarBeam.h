// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BossAttackBase.h"
#include "Templates/SubclassOf.h"
#include "BossAttack_LunarBeam.generated.h"

class AAttack_Marker;
class UNiagaraSystem;

/**
 * 보스 주변 원형 지점에 마커를 깔고 광선을 낙하시킨다.
 */
UCLASS(DisplayName = "Lunar Beam")
class ECLIPSE_API UBossAttack_LunarBeam : public UBossAttackBase
{
	GENERATED_BODY()

protected:
	virtual void OnStart() override;

	void LunarBeam_SpawnMarkers();
	void LunarBeam_IntensifyMarkers();
	void LunarBeam_Impact();

	TArray<FVector> GetLunarBeamOffsets() const;

protected:
	UPROPERTY(EditAnywhere, Category = "Settings|Combat")
	TSubclassOf<AAttack_Marker> AttackMarkerClass;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|LunarBeam")
	float LunarBeamRadius = 280.f;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|LunarBeam")
	float LunarBeamOffset = 900.f; //외곽 원까지의 거리

	UPROPERTY(EditAnywhere, Category="Settings|Combat|LunarBeam")
	float LunarBeamDamage = 50.f;

	UPROPERTY(EditAnywhere, Category = "Settings|VFX")
	TObjectPtr<UNiagaraSystem> NS_LunarBeamTrail;

	UPROPERTY(EditAnywhere, Category = "Settings|VFX")
	TObjectPtr<UNiagaraSystem> NS_LunarBeamImpact; //땅에 떨어질때 충격

	FTimerHandle LunarBeamTimer;

	TArray<FVector> LunarBeamImpactLoc;
};
