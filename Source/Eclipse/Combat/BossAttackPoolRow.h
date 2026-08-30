// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "BossAttackPoolRow.generated.h"

class UBossAttackBase;

/**
 * 보스 공격 풀
 *
 * 같은 공격이 페이즈별로 다른 가중치를 가지므로 (공격 x 페이즈) 조합마다 한 행을 둔다.
 */
USTRUCT(BlueprintType)
struct FBossAttackPoolRow : public FTableRowBase
{
	GENERATED_BODY()

	/** 이 행이 적용되는 페이즈 번호. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pool", meta = (ClampMin = "1"))
	int32 Phase = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pool")
	TSoftClassPtr<UBossAttackBase> AttackClass;

	/** 룰렛 선택 가중치. 같은 페이즈 행들의 합계가 100일 필요는 없다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pool", meta = (ClampMin = "0.0"))
	float Weight = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pool", meta = (ClampMin = "0.0"))
	float Cooldown = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pool|Distance")
	bool bUseDistanceCondition = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pool|Distance", meta = (EditCondition = "bUseDistanceCondition", ClampMin = "0.0"))
	float MinDistance = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pool|Distance", meta = (EditCondition = "bUseDistanceCondition", ClampMin = "0.0"))
	float MaxDistance = 0.f;
};
