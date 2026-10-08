// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerAttackBase.h"
#include "PlayerAttack_Basic.generated.h"

/**
 * 기본 공격. 약공격 / 강공격 / 파생 마무리. 같은 클래스에 데이터만 다르게 준다.
 */
UCLASS(meta = (DisplayName = "Basic Attack"))
class ECLIPSE_API UPlayerAttack_Basic : public UPlayerAttackBase
{
	GENERATED_BODY()
};
