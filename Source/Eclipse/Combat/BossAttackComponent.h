// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BossAttack.h"
#include "BossAttackComponent.generated.h"

class UDataTable;
class AEnemyBoss;
class UBossAttackBase;

DECLARE_MULTICAST_DELEGATE(FOnBossAttackFinished)

/**
 * 보스 공격의 선택과 실행을 담당한다.
 *
 * 공격 인스턴스를 소유하고, 풀 데이터를 근거로 하나를 골라 실행시킨다.
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

	/** 소유 보스. BeginPlay에서 한 번만 캐시한다. */
	UPROPERTY(Transient)
	TObjectPtr<AEnemyBoss> Boss;

	/** 페이즈별 공격 풀. 행 타입은 FBossAttackPoolRow. */
	UPROPERTY(EditDefaultsOnly, Category = "Boss|Attack")
	TObjectPtr<UDataTable> AttackPoolTable;


// ── 실행 ─────────────────────────────────────────────────
public:
	FOnBossAttackFinished OnAttackFinishedDelegate;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ExecuteAttack(EBossAttackType Attack);

	/** 진행 중인 공격을 끊는다. 보스 사망 등 외부 사정으로 중단할 때 쓴다. */
	void CancelCurrent();

	void NotifyAttackFinished();

	TMap<EBossAttackType, float> AttackLastUsedList;

protected:
	// 임시 브리지. 풀을 갖게 되면 인스턴스는 하나씩 캐시된다.
	UPROPERTY(Transient)
	TObjectPtr<UBossAttackBase> CurrentAttack;
};
