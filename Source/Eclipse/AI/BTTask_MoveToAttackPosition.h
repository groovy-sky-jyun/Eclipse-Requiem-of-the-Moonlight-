// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BossAttack.h"
#include "BTTask_MoveToAttackPosition.generated.h"


/**
 * 공격 타입 선택된 후, 그 공격에 최적화된 위치로 이동
 * 
 *  BloodBolt     → 플레이어로부터 1200cm 거리, 고도 유지
 *  WraithDrop    → 아레나 중앙 상단 (고도 600cm)
 *  LunarBeam     → 플레이어 정 위 600cm
 *  DamningTether → 플레이어로부터 800cm (사슬 투척 거리)
 *  MiasmaStep    → 현재 위치 유지 (텔레포트가 이동 담당)
 *  EclipseVeil   → 아레나 중앙 상단
 */
UCLASS()
class ECLIPSE_API UBTTask_MoveToAttackPosition : public UBTTaskNode
{
	GENERATED_BODY()
	

public:
	UBTTask_MoveToAttackPosition();


protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


private:
	FVector TargetPosition = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float AcceptanceRadius = 200.f;

	UPROPERTY(EditAnywhere, Category="Movement")
	float MoveSpeed = 600.f;

	FVector CalcTargetPosition(EBossAttackType Attack, APawn* Boss, APawn* Player) const;
};
