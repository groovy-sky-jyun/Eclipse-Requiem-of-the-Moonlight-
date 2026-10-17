// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_RestartPoint.h"
#include "PlayerAttackComponent.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotify_RestartPoint::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	// 몽타주 에디터 미리보기에는 공격 컴포넌트가 없다.
	AActor* Owner = MeshComp ? MeshComp->GetOwner() : nullptr;
	if (UPlayerAttackComponent* AttackComponent = Owner ? Owner->FindComponentByClass<UPlayerAttackComponent>() : nullptr)
	{
		AttackComponent->NotifyRestartPoint();
	}
}
