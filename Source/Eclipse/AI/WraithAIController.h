// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEnemyAIController.h"
#include "WraithAIController.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API AWraithAIController : public ABaseEnemyAIController
{
	GENERATED_BODY()

public:
	AWraithAIController();


protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;


public:
	static const FName BB_bIsDead;
	static const FName BB_TargetLocation;
};
