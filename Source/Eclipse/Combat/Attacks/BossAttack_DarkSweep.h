// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BossAttackBase.h"
#include "Templates/SubclassOf.h"
#include "BossAttack_DarkSweep.generated.h"

class AAttack_Marker;

/**
 * 플레이어 방향으로 직선 돌진하며 경로상의 대상을 타격한다.
 */
UCLASS(DisplayName = "Dark Sweep")
class ECLIPSE_API UBossAttack_DarkSweep : public UBossAttackBase
{
	GENERATED_BODY()

protected:
	virtual void OnStart() override;
	virtual void OnFinish() override;

	void DarkSweep_StartTelegraph();
	void DarkSweep_StartDash();
	void DarkSweep_CheckHit(const FVector& CurrentStepLoc);

protected:
	UPROPERTY(EditAnywhere, Category = "Settings|Combat")
	TSubclassOf<AAttack_Marker> AttackMarkerClass;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|DarkSweep")
	float DarkSweepDistance = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|DarkSweep")
	float DarkSweepHeight = 100.f;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|DarkSweep")
	float DarkSweepHalfWidth = 100.f;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|DarkSweep")
	float DarkSweepSpeed = 2500.f;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|DarkSweep")
	float DarkSweepDamage = 35.f;

	FVector DarkSweepDirection = FVector::ZeroVector;
	FVector DarkSweepStartLoc = FVector::ZeroVector;
	FVector DarkSweepEndLoc = FVector::ZeroVector;
	FTimerHandle DarkSweepTimer;

	bool bDarkSweepHit = false; //중복 Hit 방지
};
