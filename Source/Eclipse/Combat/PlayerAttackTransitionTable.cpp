// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAttackTransitionTable.h"
#include "Eclipse.h"

void FPlayerAttackTransitionTable::Build()
{
	Tree.Reset();

	for (const FPlayerAttackTransition& Transition : Transitions)
	{
		if (!Transition.Input.IsValid() || !Transition.ChildStep.IsValid())
		{
			UE_LOG(LogEclipse, Warning, TEXT("[AttackTransition] Empty Input or ChildStep (Parent: %s)"), *Transition.ParentStep.ToString());
			continue;
		}

		// (input, child) _EmptyTag도 태그로 사용 가능하다
		TMap<FGameplayTag, FGameplayTag>& Children = Tree.FindOrAdd(Transition.ParentStep);

		// (부모 + 입력) 조합당 자식이 하나다.
		if (Children.Contains(Transition.Input))
		{
			UE_LOG(LogEclipse, Warning, TEXT("[AttackTransition] Duplicate: %s + %s"), *Transition.ParentStep.ToString(), *Transition.Input.ToString());
			continue;
		}

		Children.Add(Transition.Input, Transition.ChildStep);
	}
}

FGameplayTag FPlayerAttackTransitionTable::FindNextStep(const FGameplayTag& CurrentStep, const FGameplayTag& Input) const
{
	if (const TMap<FGameplayTag, FGameplayTag>* Children = FindChildren(CurrentStep))
	{
		if (const FGameplayTag* Next = Children->Find(Input))
		{
			return *Next;
		}
	}

	// Child에서 없으면 가장 첫 계층에서 찾아본다. (root = EmptyTag)
	if (const TMap<FGameplayTag, FGameplayTag>* RootChildren = FindChildren(FGameplayTag::EmptyTag))
	{
		if (const FGameplayTag* Root = RootChildren->Find(Input))
		{
			return *Root;
		}
	}

	return FGameplayTag::EmptyTag;
}

const TMap<FGameplayTag, FGameplayTag>* FPlayerAttackTransitionTable::FindChildren(const FGameplayTag& Step) const
{
	// Step을 부모로 가지는 (input, child) 묶음 반환
	return Tree.Find(Step);
}
