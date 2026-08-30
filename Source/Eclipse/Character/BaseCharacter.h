// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CombatInterface.h"
#include "GameplayTagContainer.h"
#include "BaseCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, Current, float, Max);

UCLASS()
class ECLIPSE_API ABaseCharacter : public ACharacter, public ICombatInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(BlueprintAssignable, Category = "Combat")
	FOnHealthChanged OnHealthChangedDelegate;


public:
	virtual void Tick(float DeltaTime) override;

	virtual void TakeCombatDamage_Implementation(float DamageAmount, AActor* Attacker) override final;

	virtual void Die_Implementation() override final;

	virtual float GetHealth_Implementation() const override { return CurrentHealth; }
	virtual float GetMaxHealth_Implementation() const override { return MaxHealth; }

	virtual bool IsAlive_Implementation() const override { return !IsDead_Implementation(); }
	virtual bool IsDead_Implementation() const override { return bIsDead || CurrentHealth <= 0.f; }

	virtual FGameplayTag GetTeamTag_Implementation() const override { return TeamTag; }

	// 팀 판정 규칙
	static bool AreHostile(AActor* Own, AActor* Attacker);

protected:
	// bLethal으로 치명타(히트 VFX, 사운드, 데미지 넘버) / Death(경직, 넉백, 히트리액션) 구분
	virtual void OnDamaged(float DamageAmount, AActor* Attacker, bool bLethal) {};

	virtual void OnDeath() {};


protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Combat|Stats")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Combat|Stats")
	float CurrentHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Combat|Stats")
	bool bIsDead = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Combat|Tags")
	FGameplayTag TeamTag;


private:
	void SetHealth(float NewHealth);
};
