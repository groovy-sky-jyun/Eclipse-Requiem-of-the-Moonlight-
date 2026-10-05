// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyState_InputWindow.h"
#include "PlayerAttackComponent.h"

void UAnimNotifyState_InputWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	// 몽타주 에디터 미리보기에는 공격 컴포넌트가 없다.
	if (UPlayerAttackComponent* AttackComponent = GetAttackComponent(MeshComp))
	{
		AttackComponent->NotifyInputWindowBegin();
	}
}

void UAnimNotifyState_InputWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (UPlayerAttackComponent* AttackComponent = GetAttackComponent(MeshComp))
	{
		AttackComponent->NotifyInputWindowEnd();
	}
}
