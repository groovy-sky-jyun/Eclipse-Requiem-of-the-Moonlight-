// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "CombatInterface.generated.h"

UENUM(BlueprintType)
enum class EHitIntensity : uint8
{
	Light,
	Medium,
	Heavy,
	Massive
};
/** 한 번의 타격이 전달하는 정보. 받는 쪽이 필요한 값만 쓴다. */
USTRUCT(BlueprintType)
struct FCombatDamage
{
	GENERATED_BODY()

	FCombatDamage() = default;
	explicit FCombatDamage(float InDamage, float InStaggerDamage = 0.f, EHitIntensity Intensity = EHitIntensity::Light)
		: Damage(InDamage), StaggerDamage(InStaggerDamage), HitIntensity(Intensity) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0"))
	float Damage = 0.f;

	// 상대의 자세를 무너뜨리는 정도. 보스의 스태거 게이지만 이 값을 소비한다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0"))
	float StaggerDamage = 0.f;

	// 공격의 강도. 이 값에 따라 플레이어의 피격 액션이 정해진다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EHitIntensity HitIntensity = EHitIntensity::Light;
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
