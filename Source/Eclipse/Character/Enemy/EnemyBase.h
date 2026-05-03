// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "EnemyBase.generated.h"



UCLASS()
class ECLIPSE_API AEnemyBase : public ABaseCharacter
{
	GENERATED_BODY()
	
public:
	AEnemyBase();

	virtual void HandleTakeDamage_Implementation(float DamageAmount, AActor* Attacker) override;
	virtual void Die_Implementation() override;


protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|UI")
	class UWidgetComponent* HealthBarWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|AI")
	class UBehaviorTree* BehaviorTree;
};
