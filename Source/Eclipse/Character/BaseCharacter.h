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

	virtual void HandleTakeDamage_Implementation(float DamageAmount, AActor* Attacker) override;
	virtual void Die_Implementation() override;

	virtual float GetHealth_Implementation() const override { return CurrentHealth; }
	virtual float GetMaxHealth_Implementation() const override { return MaxHealth; }

	virtual bool IsAlive_Implementation() const override { return CurrentHealth > 0.f && CurrentHealth <= 100.f; }
	virtual bool IsDead_Implementation() const override { return CurrentHealth <= 0.f; }

	virtual FGameplayTag GetTeamTag_Implementation() const override { return TeamTag; }
	


protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Combat|Stats")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Combat|Stats")
	float CurrentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Combat|Tags")
	FGameplayTag TeamTag;
};
