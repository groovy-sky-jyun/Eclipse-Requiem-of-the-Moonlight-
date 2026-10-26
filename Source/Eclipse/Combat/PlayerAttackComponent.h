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
	// 입력 저장 후 재시작 시점에 새 공격으로 실행 (연계 끊김)
	AfterWindow,
	// 즉시 새 공격으로 실행
	AfterRestartPoint
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

	/** 공격 중이면 현재 공격을 끊는다. */
	void StartAttack(const FGameplayTag& StepTag);

	void EndAttack();

	void CancelAttack() const;

	bool CanCancelAttack() const;

	void NotifyInputWindowBegin();

	void NotifyInputWindowEnd();

	void NotifyHitWindowBegin();

	void NotifyHitWindowTick();

	void NotifyHitWindowEnd();

	void NotifyRestartPoint();

	// PlayerAttackComponent.h (public)
	FName GetWeaponBaseSocket() const { return WeaponBaseSocket; }
	FName GetWeaponTipSocket() const { return WeaponTipSocket; }

	bool ShouldDrawTraceDebug() const { return bDrawTraceDebug; }


protected:
	/** 공격 인스턴스 */
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Settings|Attack")
	TArray<TObjectPtr<UPlayerAttackBase>> AttackInstances;

	/** 연계 공격 트리. BeginPlay에서 Build한다. */
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Attack")
	FPlayerAttackTransitionTable TransitionTable;

	UPROPERTY(Transient)
	TMap<FGameplayTag, TObjectPtr<UPlayerAttackBase>> AttackByTag;

	// 검날 판정 기준 소켓. 손잡이 → 끝
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Weapon")
	FName WeaponBaseSocket = TEXT("FX_weapon_base");

	UPROPERTY(EditDefaultsOnly, Category = "Settings|Weapon")
	FName WeaponTipSocket = TEXT("FX_weapon_tip");

	// 검 판정 스윕 경로를 그린다. 초록 = 빗나감, 빨강 = 맞음
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDrawTraceDebug = true;


protected:
	void BuildAttackByTag();

	/** 선입력을 꺼내고 비운다. */
	FGameplayTag TakeBufferedInput();


private:
	UPROPERTY(Transient)
	TObjectPtr<UPlayerAttackBase> CurrentAttack;

	FGameplayTag CurrentStepTag;

	FGameplayTag BufferedInput;

	EAttackInputPhase InputPhase = EAttackInputPhase::BeforeWindow;



};
