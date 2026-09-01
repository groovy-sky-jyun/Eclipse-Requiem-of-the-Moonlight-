// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BossAttackBase.h"
#include "Templates/SubclassOf.h"
#include "BossAttack_SilentSlash.generated.h"

class ASlashWave;

/**
 * 짧은 연주 뒤 정적을 두고, 반달 참격을 연속 3회 내보낸다.
 */
UCLASS(DisplayName = "Silent Slash")
class ECLIPSE_API UBossAttack_SilentSlash : public UBossAttackBase
{
	GENERATED_BODY()

protected:
	virtual void OnStart() override;

	virtual void OnCancel() override;

	virtual void OnFinish() override;

	/** 플레이어를 다시 조준해 참격 한 발을 발사한다. */
	void FireSlash(int32 Index);

protected:
	UPROPERTY(EditAnywhere, Category = "Settings|Combat|SilentSlash")
	TSubclassOf<ASlashWave> SlashWaveClass;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|SilentSlash", meta = (ClampMin = "1"))
	int32 SlashCount = 3;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|SilentSlash")
	float SlashInterval = 0.4f;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|SilentSlash")
	float SlashDamage = 30.f;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|SilentSlash")
	float FinalSlashDamage = 55.f;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|SilentSlash")
	float SlashSpawnOffset = 120.f;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|SilentSlash")
	float RecoveryTime = 0.5f;

	UPROPERTY(Transient)
	TArray<TObjectPtr<ASlashWave>> ActiveWaves;

	FTimerHandle SilentSlashTimer;
};
