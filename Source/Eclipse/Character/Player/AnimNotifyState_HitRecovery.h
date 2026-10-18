// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_HitRecovery.generated.h"

/**
 * 피격 회복 구간. 시작 = 입력 저장, 끝 = 회복 시점(저장된 입력 실행)
 */
UCLASS(meta = (DisplayName = "Hit Recovery Window"))
class ECLIPSE_API UAnimNotifyState_HitRecovery : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
