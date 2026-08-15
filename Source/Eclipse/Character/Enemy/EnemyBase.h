// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "BaseEnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyBase.generated.h"



UCLASS()
class ECLIPSE_API AEnemyBase : public ABaseCharacter
{
	GENERATED_BODY()
	
public:
	AEnemyBase();

	virtual void HandleTakeDamage_Implementation(float DamageAmount, AActor* Attacker) override;
	virtual void Die_Implementation() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Controller")
	TObjectPtr<ABaseEnemyAIController> AI;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Controller")
	TObjectPtr<UBlackboardComponent> BB;


protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|UI")
	class UWidgetComponent* HealthBarWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|AI")
	class UBehaviorTree* BehaviorTree;
};
