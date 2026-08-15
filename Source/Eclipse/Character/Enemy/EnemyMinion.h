// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "EnemyMinion.generated.h"


/**
* 보스 Wraith Drop으로 생성되는 망령
* - 보스 근처 위치에서 소환
* - 빠른 속도로 플레이어에게 다가감
*/

DECLARE_MULTICAST_DELEGATE(FOnAttackFinishedDelegate);

UCLASS()
class ECLIPSE_API AEnemyMinion : public AEnemyBase
{
	GENERATED_BODY()


public:
	AEnemyMinion();


protected:
	virtual void BeginPlay() override;
	virtual void HandleTakeDamage_Implementation(float DamageAmount, AActor* Attacker) override;
	virtual void Die_Implementation() override;


public:
	void AttackStart();
	void AttackEnd();

	FOnAttackFinishedDelegate OnAttackFinishedDelegate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class AEnemyBoss* OwnerBoss;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float Damage = 50.f;


public:
	void SetDamage(float InDamage) { Damage = InDamage; }
	void SetSpeed(float InSpeed);

};