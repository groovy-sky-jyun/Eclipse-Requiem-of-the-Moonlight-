// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimNotifyState_PlayerAttack.h"
#include "AnimNotifyState_InputWindow.generated.h"

/**
 * 선입력 구간. 시작 = 구간 열기, 끝 = 전환 시점
 */
UCLASS(meta = (DisplayName = "Input Window"))
class ECLIPSE_API UAnimNotifyState_InputWindow : public UAnimNotifyState_PlayerAttack
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
