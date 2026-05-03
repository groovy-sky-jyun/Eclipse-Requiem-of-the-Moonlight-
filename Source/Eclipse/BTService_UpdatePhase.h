// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UpdatePhase.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UBTService_UpdatePhase : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_UpdatePhase();

	
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;


private:
	int32 LastKnownPhase = 1;
};
