// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/TimerHandle.h"
#include "CombatInterface.h"
#include "GameplayTagContainer.h"
#include "PlayerAttackBase.generated.h"

class UPlayerAttackComponent;
class UAnimMontage;

/** 공격 한 타의 데이터. */
USTRUCT(BlueprintType)
struct FPlayerAttackStep
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FGameplayTag StepTag;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> Montage;

	UPROPERTY(EditAnywhere)
	FCombatDamage CombatDamage;

	// 범위 판정 크기. 나중에 무기 궤적으로 교체
	UPROPERTY(EditAnywhere, Category = "Hit", meta = (ClampMin = "0.0"))
	float HitRadius = 100.f;

	UPROPERTY(EditAnywhere, Category = "Hit", meta = (ClampMin = "0.0"))
	float HitRange = 150.f;

	// 임시 : AnimNotifyState로 교체
	UPROPERTY(EditAnywhere, Category = "Timing", meta = (ClampMin = "0.0"))
	float HitTime = 0.2f;

	// 선입력을 받기 시작하는 시점. 이전 입력은 무시한다.
	UPROPERTY(EditAnywhere, Category = "Timing", meta = (ClampMin = "0.0"))
	float InputWindowStart = 0.25f;

	// 선입력이 있으면 여기서 다음 공격으로 넘어간다. 없으면 Duration까지 재생한다.
	UPROPERTY(EditAnywhere, Category = "Timing", meta = (ClampMin = "0.0"))
	float TransitionTime = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Timing", meta = (ClampMin = "0.05"))
	float Duration = 0.8f;
};

/**
 * 플레이어 공격 객체.
 */
UCLASS(Abstract, EditInlineNew, DefaultToInstanced, Blueprintable, CollapseCategories)
class ECLIPSE_API UPlayerAttackBase : public UObject
{
	GENERATED_BODY()

public:
	/** StepTag 타를 재생한다. 실행 중이면 이전 타를 끊고 이어서 재생한다. */
	void EnterStartup(UPlayerAttackComponent* InComponent, const FGameplayTag& StepTag);

	void EnterActive();

	void EnterRecovery();

	void Cancel();

	void Finish();

	virtual UWorld* GetWorld() const override;

	bool IsRunning() const { return bIsRunning; }

	TArray<FGameplayTag> GetAttackTags();


private:
	/** 몽타주 종료. 끊긴 경우(bInterrupted)는 무시한다. */
	void HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	FPlayerAttackStep GetStepDataByTag(const FGameplayTag& StepTag);


protected:
	UPROPERTY(EditAnywhere)
	TArray<FPlayerAttackStep> AttackStepData;


private:
	UPROPERTY(Transient)
	TObjectPtr<UPlayerAttackComponent> Component;

	bool bIsRunning = false;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> HitActors;

	bool bIsHitActive = false;
};
