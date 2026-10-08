// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerAttackBase.h"
#include "PlayerAttack_Special.generated.h"

/**
 * 특수 공격. 쿨다운을 쓰는 스킬. 역할과 연출은 미정
 */
UCLASS(meta = (DisplayName = "Special Attack"))
class ECLIPSE_API UPlayerAttack_Special : public UPlayerAttackBase
{
	GENERATED_BODY()
};
