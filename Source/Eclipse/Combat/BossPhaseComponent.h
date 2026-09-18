// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BossPhaseComponent.generated.h"

class AEnemyBoss;

USTRUCT(BlueprintType)
struct FBossPhaseData
{
	GENERATED_BODY()

	// 이 페이즈에 진입하는 HP 비율. 배열은 내림차순으로 넣는다. 페이즈 1은 1.0.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float EnterHealthRatio = 1.0f;
};

/**
 * 보스의 페이즈 전환 담당
 *
 * 페이즈 판정은 폴링하지 않는다. ABaseCharacter::OnHealthChangedDelegate에 구독해
 * HP가 바뀌는 순간에만 조건을 다시 본다.
 */
UCLASS(ClassGroup = (Boss), meta = (BlueprintSpawnableComponent))
class ECLIPSE_API UBossPhaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBossPhaseComponent();

protected:
	virtual void BeginPlay() override;

// ── Phase ─────────────────────────────────────────────
public:
	UFUNCTION(BlueprintCallable, Category = "Phase")
	int32 GetCurrentPhase() const { return CurrentPhase; }

	UFUNCTION(BlueprintPure, Category = "Phase")
	int32 GetPhaseCount() const { return PhaseDataTable.Num(); }

	UFUNCTION(BlueprintCallable, Category = "Phase")
	void EnterPhase(int32 NewPhase);


protected:
	/** OnHealthChangedDelegate에 묶인다. HP가 바뀔 때마다 페이즈 조건을 다시 본다. */
	UFUNCTION()
	void HandleHealthChanged(float Current, float Max);

	int32 FindPhase(float HealthRatio) const;

	/** 인덱스 0이 페이즈 1이다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Phase")
	TArray<FBossPhaseData> PhaseDataTable;

	UPROPERTY(Transient)
	TObjectPtr<AEnemyBoss> Boss;


private:
	int32 CurrentPhase = 1;
};
