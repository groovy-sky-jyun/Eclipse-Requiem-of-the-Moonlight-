// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_OrbitPlayer.generated.h"

/**
 * 공격 후 일정량 회전 위치로 이동
 * 페이즈 1: 느린 궤도 (30도)
 * 페이즈 2: 중간 궤도 (50도) + 고도 변화
 * 페이즈 3: 빠른 궤도 (70도) + 고도 변화 + 방향 무작위
 */
UCLASS()
class ECLIPSE_API UBTTask_OrbitPlayer : public UBTTaskNode
{
	GENERATED_BODY()
	

public:
	UBTTask_OrbitPlayer();


protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
    virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


private:
    // 계산한 궤도 각도
    float CachedAngle = 0.f;

    // 경과 시간
    float ElapsedTime = 0.f;

    // 타임아웃 시간 (종료 조건)
    float TimeoutDuration = 3.f;

    UPROPERTY(EditAnywhere, Category = "Orbit")
    float OrbitRadius = 1200.f;

    UPROPERTY(EditAnywhere, Category = "Orbit")
    float MoveSpeed = 700.f;

    UPROPERTY(EditAnywhere, Category = "Orbit")
    float AcceptanceRadius = 200.f;
};
