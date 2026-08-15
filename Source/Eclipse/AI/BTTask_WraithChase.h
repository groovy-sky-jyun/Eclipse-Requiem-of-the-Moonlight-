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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wraith|Chase")
	float AcceptanceRadius = 50.f;

private:
	FVector CachedPlayerLoc;
	float TaskElapsedTime;
};
