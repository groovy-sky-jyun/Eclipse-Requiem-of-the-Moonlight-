// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_PlayerAttack.generated.h"

class UPlayerAttackComponent;

/**
 * 플레이어 공격 NotifyState 공통 부모.
 * 몽타주 주인의 공격 컴포넌트를 찾아준다.
 */
UCLASS(Abstract)
class ECLIPSE_API UAnimNotifyState_PlayerAttack : public UAnimNotifyState
{
	GENERATED_BODY()

protected:
	UPlayerAttackComponent* GetAttackComponent(USkeletalMeshComponent* MeshComp) const;
};
