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
class UAnimInstance;
class ABaseCharacter;

/** 공격 진행 단계. Idle이 아니면 실행 중이다. */
UENUM(BlueprintType)
enum class EPlayerAttackState : uint8
{
	Idle,
	Startup,
	Active,
	Recovery
};

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

	// 검날 스윕 판정 구체의 반지름
	UPROPERTY(EditAnywhere, Category = "Hit", meta = (ClampMin = "0.0"))
	float HitRadius = 20.f;
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

	bool IsRunning() const { return CurrentState != EPlayerAttackState::Idle; }

	TArray<FGameplayTag> GetAttackTags();

	virtual bool CanCancel() { return true; }

	void TraceWeapon();


protected:
	virtual void OnStartup() {}
	virtual void OnActive() {}
	virtual void OnRecovery() {}
	virtual void OnCancel() {}
	virtual void OnFinish() {}

	ABaseCharacter* GetOwnerCharacter() const;

private:
	/** 몽타주 종료. 끊긴 경우(bInterrupted)는 무시한다. */
	void HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	FPlayerAttackStep GetStepDataByTag(const FGameplayTag& StepTag);

	UAnimInstance* GetAnimInstance() const;

	void StopMontage();

	TArray<FVector> GetTracePoints();

	void ApplyHit(AActor* Target);


protected:
	UPROPERTY(EditAnywhere)
	TArray<FPlayerAttackStep> AttackStepData;

	EPlayerAttackState CurrentState = EPlayerAttackState::Idle;


private:
	UPROPERTY(Transient)
	TObjectPtr<UPlayerAttackComponent> Component;

	UPROPERTY(Transient)
	TObjectPtr<UAnimMontage> CurrentMontage;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> HitActors;

	// 검날 위 검사 지점 수. 많을수록 정확하고 비용이 늘어난다.
	static constexpr int32 TraceSampleCount = 6;

	// 현재 타의 데이터. 데미지, 판정 두께에 쓴다.
	UPROPERTY(Transient)
	FPlayerAttackStep CurrentStep;

	// 이전 틱의 검사 지점 위치
	TArray<FVector> PrevTracePoints;
};
