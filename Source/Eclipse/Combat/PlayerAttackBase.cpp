// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAttackBase.h"
#include "Eclipse.h"
#include "PlayerAttackComponent.h"
#include "BaseCharacter.h"
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

	UAnimInstance* AnimInstance = GetAnimInstance();

	const FPlayerAttackStep StepData = GetStepDataByTag(StepTag);

	if (!AnimInstance || !StepData.Montage)
	{
		UE_LOG(LogPlayerAttack, Warning, TEXT("PlayerAttackBase::EnterStartup : AnimInstance or Montage missing (%s)"), *StepTag.ToString());
		return;
	}

	CurrentState = EPlayerAttackState::Startup;

	// 실행 중이면 새 몽타주가 이전 몽타주를 끊는다. 
	AnimInstance->Montage_Play(StepData.Montage);
	CurrentMontage = StepData.Montage;

	// 매개변수가 const가 아닌 참조라서 변수로 만들어 넘긴다.
	FOnMontageEnded EndDelegate = FOnMontageEnded::CreateUObject(this, &UPlayerAttackBase::HandleMontageEnded);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, StepData.Montage);

	OnStartup();
}

void UPlayerAttackBase::EnterActive()
{
	HitActors.Reset();
	CurrentState = EPlayerAttackState::Active;

	OnActive();
}

void UPlayerAttackBase::EnterRecovery()
{
	CurrentState = EPlayerAttackState::Recovery;

	OnRecovery();
}


void UPlayerAttackBase::HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// 끊긴 경우(새 공격 재생, Cancel)는 끊은 쪽에서 이미 정리했다.
	if (bInterrupted) return;


	Finish();
}

void UPlayerAttackBase::Cancel()
{
	OnCancel();

	StopMontage();

	Finish();
}

void UPlayerAttackBase::Finish()
{
	if (!Component) return;

	CurrentState = EPlayerAttackState::Idle;
	CurrentMontage = nullptr;

	OnFinish();

	Component->EndAttack();
}



// ── 헬퍼 ─────────────────────────────────────────────
UWorld* UPlayerAttackBase::GetWorld() const
{
	return Component ? Component->GetWorld() : nullptr;
}

ABaseCharacter* UPlayerAttackBase::GetOwnerCharacter() const
{
	return Component ? Cast<ABaseCharacter>(Component->GetOwner()) : nullptr;
}

UAnimInstance* UPlayerAttackBase::GetAnimInstance() const
{
	ACharacter* Character = Component ? Cast<ACharacter>(Component->GetOwner()) : nullptr;
	return (Character && Character->GetMesh()) ? Character->GetMesh()->GetAnimInstance() : nullptr;
}

void UPlayerAttackBase::StopMontage()
{
	UAnimInstance* AnimInstance = GetAnimInstance();
	if (!AnimInstance || !CurrentMontage) return;

	AnimInstance->Montage_Stop(CurrentMontage->GetDefaultBlendOutTime(), CurrentMontage);
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



