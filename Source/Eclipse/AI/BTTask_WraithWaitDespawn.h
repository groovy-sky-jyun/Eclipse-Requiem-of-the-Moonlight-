// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_WraithWaitDespawn.generated.h"

/**
 * 플레이어가 경기장 벗어났을 때 실행
 * 플레이어가 경기장 벗어났으면 WraitDuration 동안 대기
 * - 다시 돌아오면 즉시 전투 재개
 * - 시간이 지나면 소환 해제(Destroy)
 */
UCLASS()
class ECLIPSE_API UBTTask_WraithWaitDespawn : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_WraithWaitDespawn();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	FTimerHandle DespawnTimerHandle;
};
