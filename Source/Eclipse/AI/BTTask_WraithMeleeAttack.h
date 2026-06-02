// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_WraithMeleeAttack.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UBTTask_WraithMeleeAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_WraithMeleeAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	float ElapsedTime = 0.f;

	UPROPERTY(EditAnywhere, Category = "Wraith|Attack")
	float AttackRange = 200.f;

	UPROPERTY(EditAnywhere, Category = "Wraith|Attack")
	float AttackCoolDown = 1.f;
};
