// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_RestartPoint.generated.h"

/**
 * 새 공격 실행 시점. InputWindow 이후 입력은 여기서 실행하고, 이후 입력은 즉시 실행한다.
 */
UCLASS(meta = (DisplayName = "Restart Point"))
class ECLIPSE_API UAnimNotify_RestartPoint : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
