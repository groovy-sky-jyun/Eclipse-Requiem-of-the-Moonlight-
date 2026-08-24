// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "BossAttackPoolRow.generated.h"

class UBossAttackBase;

/**
 * 보스 공격 풀의 한 행. 페이즈 하나에서 공격 하나가 선택되는 조건을 기술한다.
 *
 * 같은 공격이 페이즈별로 다른 가중치를 가지므로 (공격 x 페이즈) 조합마다 한 행을 둔다.
 * 가중치는 같은 페이즈의 다른 행들과의 상대값으로만 의미가 있어 데이터 테이블에 모아둔다.
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

	/** 재사용 대기 시간(초). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pool", meta = (ClampMin = "0.0"))
	float Cooldown = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pool|Distance")
	bool bUseDistanceCondition = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pool|Distance", meta = (EditCondition = "bUseDistanceCondition", ClampMin = "0.0"))
	float MinDistance = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pool|Distance", meta = (EditCondition = "bUseDistanceCondition", ClampMin = "0.0"))
	float MaxDistance = 0.f;

	/** 전투당 최대 사용 횟수. 0이면 무제한. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pool", meta = (ClampMin = "0"))
	int32 MaxUseCount = 0;
};
