// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerAttackBase.h"
#include "PlayerAttack_Ultimate.generated.h"

/**
 * 궁극기. 판정이 끝날 때까지 무적 + 대시 캔슬 불가, Recovery부터 풀린다.
 */
UCLASS(meta = (DisplayName = "Ultimate Attack"))
class ECLIPSE_API UPlayerAttack_Ultimate : public UPlayerAttackBase
{
	GENERATED_BODY()
};
