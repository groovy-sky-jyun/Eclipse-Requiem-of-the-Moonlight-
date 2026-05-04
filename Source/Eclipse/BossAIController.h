// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BossAIController.generated.h"

class UBehaviorTreeComponent;
class UBlackboardComponent;
class UBehaviorTree;

UCLASS()
class ECLIPSE_API ABossAIController : public AAIController
{
	GENERATED_BODY()
	

public:
	ABossAIController();

	
protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;


public:
	static const FName BB_TargetActor;
	static const FName BB_CurrentPhase;
	static const FName BB_bIsInCombat;
	static const FName BB_DistanceToTarget;
	static const FName BB_SelectedAttack;
	static const FName BB_bCanReceiveDamage;
	static const FName BB_ActiveWraithCount;
	static const FName BB_UsedUltimateAttack;

	FORCEINLINE UBlackboardComponent* GetBlackboardComponent() const
	{
		return BlackboardComponent;
	}


protected:
	UPROPERTY(EditDefaultsOnly, Category="AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;


private:
	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BTComponent;

	UPROPERTY()
	UBlackboardComponent* BlackboardComponent;
};
