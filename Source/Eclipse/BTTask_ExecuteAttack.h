// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BossAttack.h"
#include "BTTask_ExecuteAttack.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UBTTask_ExecuteAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_ExecuteAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

private:
	float GetAttackDuration(EBossAttackType Attack) const;

	FTimerHandle FinishTimer;
};
