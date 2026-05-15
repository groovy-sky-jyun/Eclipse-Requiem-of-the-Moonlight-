// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_IdleHover.generated.h"

/**
 * IsInCombat = false 에서 실행
 * 공중에 둥둥 떠있음 (부유하며 대기)
 * 플레이어 감지 시 전투 전환
 */
UCLASS()
class ECLIPSE_API UBTTask_IdleHover : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_IdleHover();


protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;


private:
	float ElapsedTime = 0.f;

	UPROPERTY(EditAnywhere, Category = "Idle")
	float HoverAmplitude = 30.f;

	UPROPERTY(EditAnywhere, Category = "Idle")
	float HoverSpeed = 1.f;
};
