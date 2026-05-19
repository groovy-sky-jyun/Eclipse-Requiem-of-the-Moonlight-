// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_OrbitPlayer.generated.h"

/**
 * 공격 후 일정시간 위치 이동
 * 
 * 플레이어와 보스 사이 일직선 거리를 기준으로 경기장 원점이 있는 방향으로 대각선 이동
 * 플레이어와 보스 사이 거리가 일정 이하라면 움직이지 않음
 *
 * [이동 시 완료 조건]
 * - 목표 위치 도달
 * - 타임아웃
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
    FVector TargetPosition = FVector::ZeroVector;

    // 경과 시간(계산용)
    float ElapsedTime = 0.f;

    // 이 거리 이상이면 스트레이프
    UPROPERTY(EditAnywhere, Category = "Orbit")
    float CloseRangeThreshold = 600.f;

    // 스트레이프 이동 거리 (측면으로 얼마나 이동할지)
    UPROPERTY(EditAnywhere, Category = "Orbit")
    float StrafeDistance = 500.f;

    // 이동 속도
    UPROPERTY(EditAnywhere, Category = "Orbit")
    float MoveSpeed = 700.f;

    // 목표 도달 판정 거리
    UPROPERTY(EditAnywhere, Category = "Orbit")
    float AcceptanceRadius = 120.f;

    // 강제 완료 타임아웃
    UPROPERTY(EditAnywhere, Category = "Orbit")
    float TimeoutDuration = 3.f;

    // 목표 위치 계산 함수
    FVector CalcStrafeTarget(APawn* Boss, APawn* Player, const FVector& ArenaCenter) const;
};
