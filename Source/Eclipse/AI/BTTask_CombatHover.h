// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_CombatHover.generated.h"

/** 
 * 공격 후 숨고르는 구간
 * 제자리에서 위아래로 부유하며 짧게 대기
 * 플레이어에게 반격 타이밍
 * [페이즈별 대기 시간]
 * 1페이즈 : 2초 / 2페이즈 : 1.2초 / 3페이즈 : 0.6초
 */
UCLASS()
class ECLIPSE_API UBTTask_CombatHover : public UBTTaskNode
{
	GENERATED_BODY()


public:
    UBTTask_CombatHover();


protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
    virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


private:
    float ElapsedTime = 0.f;
    float HoverDuration = 2.f; // ExecuteTask에서 페이즈에 맞게 덮어씀

    // 부유 연출 파라미터
    UPROPERTY(EditAnywhere, Category = "CombatHover")
    float HoverAmplitude = 20.f;

    UPROPERTY(EditAnywhere, Category = "CombatHover")
    float HoverSpeed = 3.f;

    // 시작 위치 (Tick에서 Sin 오프셋 기준점으로 사용)
    FVector StartLocation = FVector::ZeroVector;
};
