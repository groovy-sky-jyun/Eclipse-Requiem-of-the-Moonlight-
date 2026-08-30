// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SelectAttack.generated.h"

/**
 * 이번에 쓸 공격을 컴포넌트에 예약시킨다.
 *
 * 풀 데이터와 선택 규칙(가중치/쿨타임/거리)은 UBossAttackComponent가 가진다.
 * 이 태스크는 BT 쪽 전제조건(보스, 타겟)만 확인하고 넘긴다.
 */
UCLASS()
class ECLIPSE_API UBTTask_SelectAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_SelectAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
