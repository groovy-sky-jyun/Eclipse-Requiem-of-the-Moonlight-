// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_GroggyRecover.generated.h"

/**
 * [스태거 발동 조건] 택1
 * 1) 누적 데미지 방식 : 짧은 시간 내 일정 데미지 초과 시 발동
 * 2) 피격 가능 구간 : 피격 가능 구간에서 공격 받은 경우 발동
 * 
 *[흐름]
 * 1. n초 동안 기절 (딜타임 제공)
 * 2. n초 이후 BB_bIsGroggy = false -> 전투 재개
 *
 * [페이즈 별 기절 시간]
 * 1페이즈 : 3.5f / 2페이즈 : 3.f / 1페이즈 : 2.5f
 */
UCLASS()
class ECLIPSE_API UBTTask_GroggyRecover : public UBTTaskNode
{
	GENERATED_BODY()


public:
    UBTTask_GroggyRecover();


protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;


private:
    float ElapsedTime = 0.f;

    // 기절 지속 시간 
    UPROPERTY(EditAnywhere, Category = "Groggy")
    float GroggyDuration = 3.5f;
};
