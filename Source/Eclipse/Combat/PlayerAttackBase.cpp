// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAttackBase.h"
#include "Eclipse.h"
#include "PlayerAttackComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"

// ── 실행 제어 ─────────────────────────────────────────────
void UPlayerAttackBase::EnterStartup(UPlayerAttackComponent* InComponent, const FGameplayTag& StepTag)
{
	if (!InComponent) return;

	Component = InComponent;

	ACharacter* Character = Component ? Cast<ACharacter>(Component->GetOwner()) : nullptr;
	UAnimInstance* AnimInstance = (Character && Character->GetMesh()) ? Character->GetMesh()->GetAnimInstance() : nullptr;

	const FPlayerAttackStep StepData = GetStepDataByTag(StepTag);

	if (!AnimInstance || !StepData.Montage)
	{
		UE_LOG(LogPlayerAttack, Warning, TEXT("PlayerAttackBase::EnterStartup : AnimInstance or Montage missing (%s)"), *StepTag.ToString());
		return;
	}

	bIsRunning = true;
	bIsHitActive = false;

	// 실행 중이면 새 몽타주가 이전 몽타주를 끊는다. 
	AnimInstance->Montage_Play(StepData.Montage);

	// 매개변수가 const가 아닌 참조라서 변수로 만들어 넘긴다.
	FOnMontageEnded EndDelegate = FOnMontageEnded::CreateUObject(this, &UPlayerAttackBase::HandleMontageEnded);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, StepData.Montage);
}

void UPlayerAttackBase::EnterActive()
{
	HitActors.Reset();
	bIsHitActive = true;
}

void UPlayerAttackBase::EnterRecovery()
{
	bIsHitActive = false;
}


void UPlayerAttackBase::HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// 다음 타나 다른 공격 몽타주가 재생되면서 끊긴 경우는 무시한다.
	if (bInterrupted) return;

	Finish();
}

void UPlayerAttackBase::Cancel()
{
	Finish();
}

void UPlayerAttackBase::Finish()
{
	if (!Component) return;

	bIsRunning = false;
	Component->EndAttack();
}



// ── 헬퍼 ─────────────────────────────────────────────
UWorld* UPlayerAttackBase::GetWorld() const
{
	return Component ? Component->GetWorld() : nullptr;
}



TArray<FGameplayTag> UPlayerAttackBase::GetAttackTags()
{
	TArray<FGameplayTag> Tags;
	for (const auto& Data : AttackStepData)
	{
		if (!Data.StepTag.IsValid())
		{
			UE_LOG(LogPlayerAttack, Warning, TEXT("PlayerAttack::GetAttackTags : Empty StepTag"));
			continue;
		}
		if (Tags.Contains(Data.StepTag))
		{
			UE_LOG(LogPlayerAttack, Warning, TEXT("PlayerAttack::GetAttackTags : Duplicate StepTag %s"),*Data.StepTag.ToString());
			continue;
		}

		Tags.Add(Data.StepTag);
	}
	return Tags;
}



FPlayerAttackStep UPlayerAttackBase::GetStepDataByTag(const FGameplayTag& StepTag)
{
	for (const auto& Data : AttackStepData)
	{
		if (StepTag == Data.StepTag)
		{
			return Data;
		}
	}

	return FPlayerAttackStep();
}



