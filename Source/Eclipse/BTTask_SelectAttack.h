// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BossAttack.h"
#include "BTTask_SelectAttack.generated.h"


UCLASS()
class ECLIPSE_API UBTTask_SelectAttack : public UBTTaskNode
{
	GENERATED_BODY()
	

public:
	UBTTask_SelectAttack();


protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


private:
	struct FAttackEntry
	{
		EBossAttackType Attack;
		float Weight;
		float Cooldown;
	};

	TArray<FAttackEntry> GetAttackPool(int32 Phase) const;

	EBossAttackType PickAttack(const TArray<FAttackEntry>& Pool, const TMap<EBossAttackType, float>& LastUsed, float Now) const;
};
