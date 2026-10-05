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
void UPlayerAttackBase::PlayStep(UPlayerAttackComponent* InComponent, const FGameplayTag& StepTag)
{
	if (!InComponent) return;

	Component = InComponent;

	ACharacter* Character = Component ? Cast<ACharacter>(Component->GetOwner()) : nullptr;
	UAnimInstance* AnimInstance = (Character && Character->GetMesh()) ? Character->GetMesh()->GetAnimInstance() : nullptr;

	const FPlayerAttackStep StepData = GetStepDataByTag(StepTag);

	if (!AnimInstance || !StepData.Montage)
	{
		UE_LOG(LogEclipse, Warning, TEXT("PlayerAttackBase::PlayStep : AnimInstance or Montage missing (%s)"), *StepTag.ToString());
		return;
	}

	// 재생에 성공할 때만 실행 중으로 둔다. 실패한 채로 실행 중이면 공격이 끝나지 않는다.
	bIsRunning = true;

	// 실행 중이면 새 몽타주가 이전 몽타주를 끊는다. 이전 종료 이벤트는 bInterrupted라 무시된다.
	AnimInstance->Montage_Play(StepData.Montage);

	// 매개변수가 const가 아닌 참조라서 변수로 만들어 넘긴다.
	FOnMontageEnded EndDelegate = FOnMontageEnded::CreateUObject(this, &UPlayerAttackBase::HandleMontageEnded);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, StepData.Montage);
}

void UPlayerAttackBase::HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// 다음 타나 다른 공격 몽타주가 재생되면서 끊긴 경우는 무시한다.
	if (bInterrupted) return;

	Finish();
}

void UPlayerAttackBase::Cancel()
{
	bIsRunning = false;
}

void UPlayerAttackBase::Finish()
{
	bIsRunning = false;
}


UWorld* UPlayerAttackBase::GetWorld() const
{
	return Component ? Component->GetWorld() : nullptr;
}


// ── 헬퍼 ─────────────────────────────────────────────
TArray<FGameplayTag> UPlayerAttackBase::GetAttackTags()
{
	TArray<FGameplayTag> Tags;
	for (const auto& Data : AttackStepData)
	{
		if (!Data.StepTag.IsValid())
		{
			UE_LOG(LogEclipse, Warning, TEXT("PlayerAttack::GetAttackTags : Empty StepTag"));
			continue;
		}
		if (Tags.Contains(Data.StepTag))
		{
			UE_LOG(LogEclipse, Warning, TEXT("PlayerAttack::GetAttackTags : Duplicate StepTag %s"),*Data.StepTag.ToString());
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



