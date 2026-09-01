// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BossAttackBase.h"
#include "Templates/SubclassOf.h"
#include "BossAttack_EclipseVeil.generated.h"

class ASlashBeam;

/**
 * 암흑 안개를 펼친 뒤 라운드별로 참격을 전개하는 페이즈 3 전용 기술.
 */
UCLASS(DisplayName = "Eclipse Veil")
class ECLIPSE_API UBossAttack_EclipseVeil : public UBossAttackBase
{
	GENERATED_BODY()

protected:
	virtual void OnStartup() override;
	virtual void OnFinish() override;

	void EclipseVeil_StartFog();
	void EclipseVeil_ExecuteRound(int32 Round);
	void EclipseVeil_SpawnSlash(FVector Center, float AngleDeg, float Damage);

	struct FSlashConfig 
	{ 
		FVector offset;
		float AngleDeg;
	};

	TArray<FSlashConfig> GetSlashConfigs(int32 Round) const;

protected:
	UPROPERTY(EditAnywhere, Category="Settings|Combat|EclipseVeil")
	TSubclassOf<ASlashBeam> SlashBeamClass;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|EclipseVeil")
	float EclipseDamage = 40.f;

	FTimerHandle EclipseVeilTimer;
	int32 EclipseCurrentRound = 0;
};
