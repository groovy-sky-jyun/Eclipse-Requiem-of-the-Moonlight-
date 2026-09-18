// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "CombatInterface.generated.h"

/** 한 번의 타격이 전달하는 정보. 받는 쪽이 필요한 값만 쓴다. */
USTRUCT(BlueprintType)
struct FCombatDamage
{
	GENERATED_BODY()

	FCombatDamage() = default;
	explicit FCombatDamage(float InDamage, float InStaggerDamage = 0.f)
		: Damage(InDamage), StaggerDamage(InStaggerDamage) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0"))
	float Damage = 0.f;

	// 보스만 사용한다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0"))
	float StaggerDamage = 0.f;
};

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
	void TakeCombatDamage(const FCombatDamage& DamageInfo, AActor* Attacker);

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
