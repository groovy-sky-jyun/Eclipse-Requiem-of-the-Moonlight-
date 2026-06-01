// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BaseEnemyAIController.generated.h"

class UBehaviorTreeComponent;
class UBlackboardComponent;
class UBehaviorTree;

UCLASS()
class ECLIPSE_API ABaseEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	ABaseEnemyAIController();


protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;


public:
	static const FName BB_TargetActor;
	static const FName BB_CurrentPhase;
	static const FName BB_bIsInCombat;
	static const FName BB_DistanceToTarget;
	static const FName BB_bIsPlayerInRange;
	static const FName BB_CenterLocation;
	static const FName BB_bIsStaggered;

	FORCEINLINE UBlackboardComponent* GetBlackboardComponent() const { return BlackboardComponent; }


protected:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

	UPROPERTY()	TObjectPtr<UBehaviorTreeComponent> BTComponent;

	UPROPERTY()	UBlackboardComponent* BlackboardComponent;
};
