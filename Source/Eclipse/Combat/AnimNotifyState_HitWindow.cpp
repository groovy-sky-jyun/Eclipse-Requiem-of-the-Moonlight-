// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyState_HitWindow.h"
#include "PlayerAttackComponent.h"

void UAnimNotifyState_HitWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	// GetAttackComponent(MeshComp) -> 현재 공격 Active
}

void UAnimNotifyState_HitWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	// GetAttackComponent(MeshComp) -> 현재 공격 Recovery
}
