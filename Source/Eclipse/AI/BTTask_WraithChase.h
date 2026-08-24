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
	FVector CachedPlayerLoc = FVector::ZeroVector;

	/** 초기화가 없으면 첫 TickTask에서 쓰레기 값과 비교하게 된다. */
	float TaskElapsedTime = 0.f;
};
