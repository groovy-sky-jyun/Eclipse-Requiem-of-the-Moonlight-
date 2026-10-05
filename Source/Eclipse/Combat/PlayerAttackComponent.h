// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerAttackBase.h"
#include "PlayerAttackTransitionTable.h"
#include "GameplayTagContainer.h"
#include "PlayerAttackComponent.generated.h"

/** 한 타 안에서 입력을 어떻게 처리할지 나누는 구간. */
UENUM(BlueprintType)
enum class EAttackInputPhase : uint8
{
	// 입력 무시
	BeforeWindow,
	// 입력 저장 후 전환 시점에 실행
	InWindow,
	// 입력 저장 후 공격이 끝나면 새 공격으로 실행
	AfterWindow
};

/**
 * 플레이어 공격 입력, 선입력, 쿨다운, 실행을 담당한다.
 *
 * 개별 공격의 실행 내용은 UPlayerAttackBase 파생 클래스가 가진다.
 */
UCLASS(ClassGroup = (Player), meta = (BlueprintSpawnableComponent))
class ECLIPSE_API UPlayerAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerAttackComponent();

protected:
	virtual void BeginPlay() override;


public:
	void RequestAttack(FGameplayTag InputTag);

	bool IsAttacking() const;



protected:
	/** 공격 인스턴스 */
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Settings|Attack")
	TArray<TObjectPtr<UPlayerAttackBase>> AttackInstances;

	/** 연계 공격 트리. BeginPlay에서 Build한다. */
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Attack")
	FPlayerAttackTransitionTable TransitionTable;

	/** 전환 시점 이후 입력 중, 공격 종료 전 이 시간 안의 입력만 새 공격으로 시작한다. */
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Attack", meta = (ClampMin = "0.0"))
	float InputBufferDuration = 0.2f;


private:
	void BuildAttackByTag();

	/** StepTag 타를 재생한다. 다른 공격 객체로 넘어가면 현재 공격을 끊는다. */
	void StartAttack(const FGameplayTag& StepTag);

	void EndAttack();

	/** 선입력을 꺼내고 비운다. */
	FGameplayTag TakeBufferedInput();

private:
	UPROPERTY(Transient)
	TObjectPtr<UPlayerAttackBase> CurrentAttack;

	UPROPERTY(Transient)
	TMap<FGameplayTag, TObjectPtr<UPlayerAttackBase>> AttackByTag;

	FGameplayTag CurrentStepTag;

	FGameplayTag BufferedInput;

	float BufferedInputTime = 0.f;

public:
	void NotifyInputWindowBegin();
	void NotifyInputWindowEnd();

private:
	EAttackInputPhase InputPhase = EAttackInputPhase::BeforeWindow;
};
