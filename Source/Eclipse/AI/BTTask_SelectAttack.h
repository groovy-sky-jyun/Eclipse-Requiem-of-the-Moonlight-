// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BossAttack.h"
#include "BTTask_SelectAttack.generated.h"

class AEnemyBoss;

UCLASS()
class ECLIPSE_API UBTTask_SelectAttack : public UBTTaskNode
{
	GENERATED_BODY()
	

public:
	UBTTask_SelectAttack();


protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	//virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	//float ElapsedTime = 0.f;
	//float TimeoutDuration = 5.f;

	struct FAttackEntry
	{
		EBossAttackType Attack;
		float Weight = 0.f;
		float Cooldown = 0.f;
		bool bHasCondition = true;
		float MinDist = 0.f;
		float MaxDist = 99999.f;
	};

	TArray<FAttackEntry> GetAttackPool(int32 Phase) const;

	EBossAttackType PickAttack(AEnemyBoss* Boss, APawn* Player, const TArray<FAttackEntry>& Pool, const TMap<EBossAttackType, float>& LastUsed, float Now) const;
};
