// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEnemyAIController.h"
#include "BossAIController.generated.h"

class AEnemyBoss;

UCLASS()
class ECLIPSE_API ABossAIController : public ABaseEnemyAIController
{
	GENERATED_BODY()
	

public:
	ABossAIController();

	
protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	/** 전투 시작 알림. 이때부터 BT의 공격 분기가 열린다. */
	UFUNCTION()
	void HandleBattleStarted(AEnemyBoss* InBoss);


public:
	static const FName BB_SelectedAttack;
	static const FName BB_bCanReceiveDamage;
	static const FName BB_ActiveWraithCount;
	static const FName BB_OrbitAngle;
	static const FName BB_bIsGroggy;
	static const FName BB_CurrentPhase;
	static const FName BB_bIsInCombat;

};
