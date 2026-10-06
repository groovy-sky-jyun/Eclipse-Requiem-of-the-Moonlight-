// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAttackComponent.h"
#include "Eclipse.h"
#include "PlayerAttackBase.h"

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
			UE_LOG(LogPlayerAttack, Warning, TEXT("PlayerAttackComponent::BuildAttackByTag : Empty slot in AttackInstances"));
			continue;
		}

		for (const FGameplayTag& Tag : Instance->GetAttackTags())
		{
			if (AttackByTag.Contains(Tag))
			{
				UE_LOG(LogPlayerAttack, Warning, TEXT("PlayerAttackComponent::BuildAttackByTag : Duplicate StepTag"));
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
		// 실행은 전환 시점(InputWindow 끝)에 연계로 한다. 마지막 입력만 남긴다.
		BufferedInput = InputTag;
		break;

	case EAttackInputPhase::AfterWindow:
		// 연계는 끊기고, 남은 Recovery를 끊고 바로 새 공격을 시작한다.
		StartAttack(TransitionTable.FindNextStep(FGameplayTag::EmptyTag, InputTag));
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
		UE_LOG(LogPlayerAttack, Warning, TEXT("PlayerAttackComponent::StartAttack : No attack for step %s"), *StepTag.ToString());
		return;
	}

	// Cancel이 EndAttack으로 상태를 초기화하므로, 새 공격 상태는 그 뒤에 넣는다.
	if (IsAttacking())
	{
		CurrentAttack->Cancel();
	}

	CurrentAttack = NextAttack;
	CurrentStepTag = StepTag;
	InputPhase = EAttackInputPhase::BeforeWindow;

	NextAttack->EnterStartup(this, StepTag);
}

void UPlayerAttackComponent::EndAttack()
{
	CurrentAttack = nullptr;
	CurrentStepTag = FGameplayTag::EmptyTag;
	TakeBufferedInput();
	InputPhase = EAttackInputPhase::BeforeWindow;
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

void UPlayerAttackComponent::NotifyHitWindowBegin()
{
	if (!IsAttacking()) return;

	CurrentAttack->EnterActive();
}

void UPlayerAttackComponent::NotifyHitWindowEnd()
{
	if (!IsAttacking()) return;

	CurrentAttack->EnterRecovery();
}

// ── 헬퍼 ─────────────────────────────────────────────
FGameplayTag UPlayerAttackComponent::TakeBufferedInput()
{
	const FGameplayTag Input = BufferedInput;
	BufferedInput = FGameplayTag::EmptyTag;
	return Input;
}
