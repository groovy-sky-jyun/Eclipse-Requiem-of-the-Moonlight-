// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CombatInterface.h"
#include "GameplayTagContainer.h"
#include "BaseCharacter.generated.h"

UCLASS()
class ECLIPSE_API ABaseCharacter : public ACharacter, public ICombatInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/**
	 * 피격 파이프라인. 가드와 순서를 여기서만 결정한다.
	 * 클래스별 피격 반응은 OnDamaged()를 오버라이드
	 */
	virtual void HandleTakeDamage_Implementation(float DamageAmount, AActor* Attacker) override final;

	/**
	 * 중복 호출 차단
	 * 클래스별 사망 처리는 HandleDeath()를 오버라이드
	 */
	virtual void Die_Implementation() override final;

	virtual float GetHealth_Implementation() const override { return CurrentHealth; }
	virtual float GetMaxHealth_Implementation() const override { return MaxHealth; }

	virtual bool IsAlive_Implementation() const override { return !IsDead_Implementation(); }
	virtual bool IsDead_Implementation() const override { return bIsDead || CurrentHealth <= 0.f; }

	virtual FGameplayTag GetTeamTag_Implementation() const override { return TeamTag; }

	/**
	 * 두 액터가 서로 적대적인가. 팀 판정 규칙은 이 함수 하나에만 있다.
	 *
	 * 데미지 차단은 HandleTakeDamage가 알아서 하므로 공격 코드가 부를 일은 보통 없다.
	 * 공격자 쪽에서 부를 이유는 "때렸다"에 딸린 부수 효과(콤보 증가, 히트스톱 등)를
	 * 아군에게는 일으키지 않기 위해서다.
	 *
	 * 한쪽이라도 팀 태그가 비어 있으면 적대로 본다.
	 * 태그를 안 꽂아둔 액터가 조용히 무적이 되는 쪽이 훨씬 나쁘다.
	 */
	UFUNCTION(BlueprintPure, Category = "Combat")
	static bool AreHostile(AActor* A, AActor* B);



protected:
	/**
	 * 클래스별 피격 반응.
	 * 데미지가 실제로 적용된 직후에만 불린다. 죽었거나 피격 불가 상태면 도달하지 않는다.
	 * 이 시점의 CurrentHealth는 이미 차감된 값이고, 아직 Die()는 불리지 않았다.
	 *
	 * bLethal이면 곧바로 Die()가 이어진다.
	 * 치명타에도 필요한 것(히트 VFX, 사운드, 데미지 넘버)과
	 * 죽으면 의미 없는 것(경직, 넉백, 히트리액션)을 이 값으로 갈라야 한다.
	 */
	virtual void OnDamaged(float DamageAmount, AActor* Attacker, bool bLethal) {}

	/** 클래스별 사망 처리.  */
	virtual void HandleDeath();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Combat|Stats")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Combat|Stats")
	float CurrentHealth;

	/**
	 * 사망 처리가 이미 끝났는지 여부.
	 * 체력 0 외에 디스폰 등 비피격 사망 경로가 있으므로 체력과 별도로 둔다.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Combat|Stats")
	bool bIsDead = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Combat|Tags")
	FGameplayTag TeamTag;
};
