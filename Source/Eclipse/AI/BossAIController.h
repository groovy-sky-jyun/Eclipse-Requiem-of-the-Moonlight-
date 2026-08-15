// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEnemyAIController.h"
#include "BossAIController.generated.h"

UCLASS()
class ECLIPSE_API ABossAIController : public ABaseEnemyAIController
{
	GENERATED_BODY()
	

public:
	ABossAIController();

	
protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;


public:
	static const FName BB_SelectedAttack;
	static const FName BB_bCanReceiveDamage;
	static const FName BB_ActiveWraithCount;
	static const FName BB_OrbitAngle;
	static const FName BB_bIsStaggered;
	static const FName BB_CurrentPhase;
	static const FName BB_bIsInCombat;

};
