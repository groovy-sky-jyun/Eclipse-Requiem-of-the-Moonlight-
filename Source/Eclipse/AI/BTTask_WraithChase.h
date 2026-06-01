// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_WraithChase.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UBTTask_WraithChase : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_WraithChase();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere, Category="Wraith|Chase")
	float AttackRange = 200.f;

	UPROPERTY(EditAnywhere, Category = "Wraith|Chase")
	float MoveSpeed = 400.f;
};
