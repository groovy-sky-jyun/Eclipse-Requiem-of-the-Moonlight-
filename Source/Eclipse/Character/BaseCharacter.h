// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CombatInterface.h"
#include "GameplayTagContainer.h"
#include "BaseCharacter.generated.h"

class ABaseCharacter;
class UAnimMontage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, Current, float, Max);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnDeathMotionFinished, ABaseCharacter*);

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

	// 사망 모션이 끝났을 때. 몽타주가 없으면 사망 직후 방송된다.
	FOnDeathMotionFinished OnDeathMotionFinishedDelegate;


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
	// 피드백(히트 VFX, 사운드, 데미지 넘버)은 항상 재생한다.
	// bLethal이면 리액션(경직, 넉백, 피격 모션)은 생략한다.
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

	// Enable Auto Blend Out을 꺼야 쓰러진 자세로 남는다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Combat|Death")
	TObjectPtr<UAnimMontage> DeathMontage;


private:
	void SetHealth(float NewHealth);

	void PlayDeathMontage();
	void HandleDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted);
};
