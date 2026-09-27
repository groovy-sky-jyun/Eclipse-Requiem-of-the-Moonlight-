// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "CombatInterface.generated.h"

/** 피격자가 보일 반응. 뒤로 갈수록 등급이 높고, 높은 등급이 낮은 등급을 덮어쓴다. */
UENUM(BlueprintType)
enum class EHitReaction : uint8
{
	// 반응 없음. 체력만 깎는다.
	None,
	// 상체만 흔들린다. 이동과 공격을 막지 않는다.
	Flinch,
	// 밀려난다. 짧은 입력 잠금.
	Knockback,
	// 주저앉았다 일어난다. 입력 잠금과 기상 무적.
	Knockdown
};

/** 한 번의 타격이 전달하는 정보. 받는 쪽이 필요한 값만 쓴다. */
USTRUCT(BlueprintType)
struct FCombatDamage
{
	GENERATED_BODY()

	FCombatDamage() = default;
	explicit FCombatDamage(float InDamage, float InStaggerDamage = 0.f, EHitReaction InHitReaction = EHitReaction::None)
		: Damage(InDamage), StaggerDamage(InStaggerDamage), HitReaction(InHitReaction) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0"))
	float Damage = 0.f;

	// 상대의 자세를 무너뜨리는 정도. 보스의 스태거 게이지만 이 값을 소비한다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0"))
	float StaggerDamage = 0.f;

	// 공격이 요청하는 반응. 최종 판단은 피격자가 하며, 지금은 플레이어만 해석한다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EHitReaction HitReaction = EHitReaction::None;
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
