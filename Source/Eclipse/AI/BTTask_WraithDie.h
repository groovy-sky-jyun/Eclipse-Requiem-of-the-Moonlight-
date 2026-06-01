// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_WraithDie.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UBTTask_WraithDie : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_WraithDie();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
