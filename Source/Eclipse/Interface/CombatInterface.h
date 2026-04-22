// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "CombatInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ECLIPSE_API ICombatInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Combat")
	void HandleTakeDamage(float DamageAmount, AActor* Attacker); //데미지 받음

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	void Die();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	bool IsAlive() const; 

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	bool IsDead() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	float GetHealth() const; 

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	FGameplayTag GetTeamTag() const;
};
