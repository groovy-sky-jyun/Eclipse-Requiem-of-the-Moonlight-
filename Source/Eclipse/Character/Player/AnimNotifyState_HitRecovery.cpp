// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyState_HitRecovery.h"
#include "PlayerCharacter.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotifyState_HitRecovery::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	// 몽타주 에디터 미리보기에는 플레이어가 없다.
	if (APlayerCharacter* Player = MeshComp ? Cast<APlayerCharacter>(MeshComp->GetOwner()) : nullptr)
	{
		Player->NotifyHitRecoveryBegin();
	}
}

void UAnimNotifyState_HitRecovery::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (APlayerCharacter* Player = MeshComp ? Cast<APlayerCharacter>(MeshComp->GetOwner()) : nullptr)
	{
		Player->NotifyHitRecoveryEnd();
	}
}
