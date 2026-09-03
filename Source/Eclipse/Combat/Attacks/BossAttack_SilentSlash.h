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

public:
	UBossAttack_SilentSlash();

protected:
	virtual void OnStartup() override;

	virtual void OnActive() override;

	virtual void OnRecovery() override;

	virtual void OnCancel() override;

	virtual void OnFinish() override;

	/** 플레이어를 다시 조준해 참격 한 발을 발사한다. */
	void FireSlash(int32 Index);

	/** 참격이 도달할 시점의 플레이어 위치. 마지막 타가 쓴다. */
	FVector PredictAimLocation(const APawn* Player) const;

	/** Active 시작부터 해당 타가 나가기까지의 시간. */
	float GetSlashDelay(int32 Index) const;

protected:
	UPROPERTY(EditAnywhere, Category = "Settings|Combat|SilentSlash")
	TSubclassOf<ASlashWave> SlashWaveClass;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|SilentSlash", meta = (ClampMin = "1"))
	int32 SlashCount = 3;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|SilentSlash")
	float SlashInterval = 0.5f;

	/** 마지막 타 직전 간격. 길게 잡아 방향을 바꿀 틈을 준다. */
	UPROPERTY(EditAnywhere, Category = "Settings|Combat|SilentSlash")
	float FinalSlashDelay = 0.65f;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|SilentSlash")
	float SlashDamage = 30.f;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|SilentSlash")
	float FinalSlashDamage = 60.f;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|SilentSlash")
	float SlashSpawnOffset = 120.f;

	/** 마지막 타가 앞서 조준하는 비율. 1이면 완벽히 맞혀 회피할 수 없다. */
	UPROPERTY(EditAnywhere, Category = "Settings|Combat|SilentSlash", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float LeadFactor = 0.7f;

	UPROPERTY(Transient)
	TArray<TObjectPtr<ASlashWave>> ActiveWaves;

	FTimerHandle SilentSlashTimer;
};
