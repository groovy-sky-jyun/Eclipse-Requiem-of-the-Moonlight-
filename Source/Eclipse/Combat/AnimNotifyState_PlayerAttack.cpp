// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyState_PlayerAttack.h"
#include "PlayerAttackComponent.h"
#include "Components/SkeletalMeshComponent.h"

UPlayerAttackComponent* UAnimNotifyState_PlayerAttack::GetAttackComponent(USkeletalMeshComponent* MeshComp) const
{
	AActor* Owner = MeshComp ? MeshComp->GetOwner() : nullptr;
	return Owner ? Owner->FindComponentByClass<UPlayerAttackComponent>() : nullptr;
}
