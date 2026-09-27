// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BossPhaseComponent.generated.h"

class AEnemyBoss;

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

	UFUNCTION(BlueprintCallable, Category = "Phase")
	void EnterPhase(int32 NewPhase);


protected:
	/** OnHealthChangedDelegate에 묶인다. HP가 바뀔 때마다 페이즈 조건을 다시 본다. */
	UFUNCTION()
	void HandleHealthChanged(float Current, float Max);

	int32 FindPhase(float HealthRatio) const;

	UPROPERTY(Transient)
	TObjectPtr<AEnemyBoss> Boss;


private:
	int32 CurrentPhase = 1;
};
