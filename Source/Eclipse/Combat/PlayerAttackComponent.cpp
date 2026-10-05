// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAttackComponent.h"
#include "Eclipse.h"
#include "PlayerAttackBase.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"

UPlayerAttackComponent::UPlayerAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	TransitionTable.Build();

	BuildAttackByTag();

	InputPhase = EAttackInputPhase::BeforeWindow;
}

void UPlayerAttackComponent::BuildAttackByTag()
{
	AttackByTag.Reset();

	for (UPlayerAttackBase* Instance : AttackInstances)
	{
		if (!Instance)
		{
			UE_LOG(LogEclipse, Warning, TEXT("PlayerAttackComponent::BuildAttackByTag : Empty slot in AttackInstances"));
			continue;
		}

		for (const FGameplayTag& Tag : Instance->GetAttackTags())
		{
			if (AttackByTag.Contains(Tag))
			{
				UE_LOG(LogEclipse, Warning, TEXT("PlayerAttackComponent::BuildAttackByTag : Duplicate StepTag"));
				continue;
			}
			AttackByTag.Add(Tag, Instance);
		}
	}
}

// ── 입력 ─────────────────────────────────────────────
void UPlayerAttackComponent::RequestAttack(FGameplayTag InputTag)
{
	// 공격 중이 아니라면 새로운 공격 바로 실행
	if (!IsAttacking())
	{
		StartAttack(TransitionTable.FindNextStep(FGameplayTag::EmptyTag, InputTag));
		return;
	}

	// 공격 중이라면 입력 구간에 따라 동작이 나뉜다.
	switch (InputPhase)
	{
	case EAttackInputPhase::BeforeWindow:
		return;

	case EAttackInputPhase::InWindow:
	case EAttackInputPhase::AfterWindow:
		// 실행은 전환 시점 / 공격 종료 알림에서 한다. 마지막 입력만 남긴다.
		BufferedInput = InputTag;
		BufferedInputTime = GetWorld()->GetTimeSeconds();
		break;
	}
}

bool UPlayerAttackComponent::IsAttacking() const
{
	return CurrentAttack && CurrentAttack->IsRunning();
}

// ── 실행 ─────────────────────────────────────────────
void UPlayerAttackComponent::StartAttack(const FGameplayTag& StepTag)
{
	UPlayerAttackBase* NextAttack = AttackByTag.FindRef(StepTag);
	if (!NextAttack)
	{
		UE_LOG(LogEclipse, Warning, TEXT("PlayerAttackComponent::StartAttack : No attack for step %s"), *StepTag.ToString());
		return;
	}

	CurrentStepTag = StepTag;
	InputPhase = EAttackInputPhase::BeforeWindow;

	if (IsAttacking())
	{
		CurrentAttack->Cancel();
	}

	CurrentAttack = NextAttack;
	NextAttack->PlayStep(this, StepTag);
}

void UPlayerAttackComponent::EndAttack()
{
	//종료시점에 선입력이 있다면 새로 실행
	if (BufferedInput.IsValid())
	{
		StartAttack(TransitionTable.FindNextStep(FGameplayTag::EmptyTag, TakeBufferedInput()));
	}
}

void UPlayerAttackComponent::NotifyInputWindowBegin()
{
	// 공격이 아닌 몽타주, 끝난 공격의 몽타주에서 온 알림은 무시한다.
	if (!IsAttacking()) return;

	InputPhase = EAttackInputPhase::InWindow;
}

void UPlayerAttackComponent::NotifyInputWindowEnd()
{
	if (!IsAttacking()) return;

	InputPhase = EAttackInputPhase::AfterWindow;
	
	//전환시점에 선입력이 있다면 연계로 실행
	if (BufferedInput.IsValid())
	{
		StartAttack(TransitionTable.FindNextStep(CurrentStepTag, TakeBufferedInput()));
	}
}

// ── 헬퍼 ─────────────────────────────────────────────
FGameplayTag UPlayerAttackComponent::TakeBufferedInput()
{
	const FGameplayTag Input = BufferedInput;
	BufferedInput = FGameplayTag::EmptyTag;
	return Input;
}
