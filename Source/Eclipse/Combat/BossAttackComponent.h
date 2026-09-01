// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BossAttack.h"
#include "BossAttackPoolRow.h"
#include "BossAttackBase.h"   // TSubclassOf<UBossAttackBase>가 완전한 정의를 요구한다
#include "BossAttackComponent.generated.h"

class UDataTable;
class AEnemyBoss;
class UBossAttackBase;

DECLARE_MULTICAST_DELEGATE(FOnBossAttackFinished)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBossAttackStateChanged, EBossAttackState)

/**
 * 보스 공격 선택과 실행을 담당한다.
 *
 * 개별 공격의 실행 내용은 UBossAttackBase 파생 클래스가 가진다.
 */
UCLASS(ClassGroup = (Boss), meta = (BlueprintSpawnableComponent))
class ECLIPSE_API UBossAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBossAttackComponent();

protected:
	virtual void BeginPlay() override;

	/** 실행 중인 공격에 틱을 전달한다. */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** BeginPlay에서 AttackPoolTable을 페이즈별로 갈라 캐시한다. */
	void CacheAttackPool();


public:
	FOnBossAttackFinished OnAttackFinishedDelegate;
	FOnBossAttackStateChanged OnAttackStateChangedDelegate;

	TSubclassOf<UBossAttackBase> SelectAttack(int32 Phase, APawn* Target);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ExecuteAttack();

	/** 실행 중인 공격의 단계. 공격이 없으면 Idle이다. */
	EBossAttackState GetAttackState() const;

	/** 예열 중인 공격만 취소한다. 판정이 나간 뒤에는 실패한다. */
	bool TryCancelWindupAttack();

	void CancelCurrent();

	void NotifyAttackFinished();

	/** 실행 중인 공격이 단계를 넘길 때 호출된다. */
	void NotifyAttackStateChanged(EBossAttackState NewState);

	/** SelectAttack이 예약해둔 공격이 있는지 확인 */
	UFUNCTION(BlueprintPure, Category = "Combat")
	bool HasPendingAttack() const { return PendingAttackClass != nullptr; }


protected:
	/** 소유 보스 */
	UPROPERTY(Transient)
	TObjectPtr<AEnemyBoss> Boss;

	/** 페이즈별 공격 풀 */
	UPROPERTY(EditDefaultsOnly, Category = "Boss|Attack")
	TObjectPtr<UDataTable> AttackPoolTable;

	TMap<int32, TArray<FBossAttackPoolRow>> PoolCacheByPhase;

	/** 공격별 마지막 사용 시각. 쿨타임 판정용. */
	TMap<TSubclassOf<UBossAttackBase>, float> LastUsedTimeList;

	UPROPERTY(Transient)
	TSubclassOf<UBossAttackBase> PendingAttackClass;

	UPROPERTY(Transient)
	TObjectPtr<UBossAttackBase> CurrentAttack;
};
