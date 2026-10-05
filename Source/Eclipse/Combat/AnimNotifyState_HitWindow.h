// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimNotifyState_PlayerAttack.h"
#include "AnimNotifyState_HitWindow.generated.h"

/**
 * 공격 판정 구간(Active). 시작 = Active, 끝 = Recovery
 */
UCLASS(meta = (DisplayName = "Hit Window"))
class ECLIPSE_API UAnimNotifyState_HitWindow : public UAnimNotifyState_PlayerAttack
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
