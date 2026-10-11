// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAttack_Ultimate.h"
#include "Eclipse.h"
#include "BaseCharacter.h"

void UPlayerAttack_Ultimate::OnStartup()
{
	SetOwnerInvincible(true);
}

void UPlayerAttack_Ultimate::OnRecovery()
{
	SetOwnerInvincible(false);
}

// Recovery 전에 끊긴 경우에도 무적이 남지 않도록 해제한다.
void UPlayerAttack_Ultimate::OnFinish()
{
	SetOwnerInvincible(false);
}

bool UPlayerAttack_Ultimate::CanCancel()
{
	return CurrentState == EPlayerAttackState::Recovery;
}

void UPlayerAttack_Ultimate::SetOwnerInvincible(bool bInvincible)
{
	if (ABaseCharacter* OwnerCharacter = GetOwnerCharacter())
	{
		OwnerCharacter->SetInvincible(bInvincible);
	}
}
