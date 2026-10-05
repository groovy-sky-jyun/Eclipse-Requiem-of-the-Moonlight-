// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerAttackBase.h"
#include "PlayerAttack_Combo.generated.h"

/**
 * 약공격 / 강공격. 같은 클래스에 데이터만 다르게 준다.
 */
UCLASS(meta = (DisplayName = "Combo"))
class ECLIPSE_API UPlayerAttack_Combo : public UPlayerAttackBase
{
	GENERATED_BODY()
};
