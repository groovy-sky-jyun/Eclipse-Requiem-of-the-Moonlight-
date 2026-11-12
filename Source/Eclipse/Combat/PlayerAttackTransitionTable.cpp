// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAttackTransitionTable.h"
#include "Eclipse.h"

void FPlayerAttackTransitionTable::ValidateRoutes() const
{
	for (int32 RouteIndex = 0; RouteIndex < Routes.Num(); ++RouteIndex)
	{
		const FPlayerComboRoute& Route = Routes[RouteIndex];

		for (int32 StepIndex = 0; StepIndex < Route.Steps.Num(); ++StepIndex)
		{
			const FPlayerComboStep& Step = Route.Steps[StepIndex];
			if (!Step.Input.IsValid() || !Step.Attack.IsValid())
			{
				UE_LOG(LogPlayerAttack, Warning, TEXT("[Combo] Empty Input or Attack (%s, Step %d)"), *Route.RouteName.ToString(), StepIndex + 1);
			}

			// 앞 루트와 입력 순서가 같은데 공격이 다르면 앞 루트가 우선된다.
			for (int32 OtherIndex = 0; OtherIndex < RouteIndex; ++OtherIndex)
			{
				const FPlayerComboRoute& Other = Routes[OtherIndex];
				if (Other.Steps.Num() > StepIndex
					&& HasSameInputs(Route, Other, StepIndex + 1)
					&& Other.Steps[StepIndex].Attack != Step.Attack)
				{
					UE_LOG(LogPlayerAttack, Warning, TEXT("[Combo] Conflict: %s and %s at Step %d"), *Other.RouteName.ToString(), *Route.RouteName.ToString(), StepIndex + 1);
					break;
				}
			}
		}
	}
}

FPlayerComboPosition FPlayerAttackTransitionTable::FindNextStep(const FPlayerComboPosition& Current, const FGameplayTag& Input) const
{
	if (Current.IsValid() && Routes.IsValidIndex(Current.RouteIndex))
	{
		const FPlayerComboPosition Next = FindContinuation(Current, Input);
		if (Next.IsValid())
		{
			return Next;
		}
	}

	// 이어지는 타가 없으면 1타부터 찾는다.
	return FindContinuation(FPlayerComboPosition(), Input);
}

FGameplayTag FPlayerAttackTransitionTable::GetAttack(const FPlayerComboPosition& Position) const
{
	if (!Position.IsValid() || !Routes.IsValidIndex(Position.RouteIndex))
	{
		return FGameplayTag::EmptyTag;
	}

	const TArray<FPlayerComboStep>& Steps = Routes[Position.RouteIndex].Steps;
	return Steps.IsValidIndex(Position.StepIndex) ? Steps[Position.StepIndex].Attack : FGameplayTag::EmptyTag;
}

#if WITH_EDITOR
void FPlayerAttackTransitionTable::RefreshInputToAttackTexts()
{
	for (FPlayerComboRoute& Route : Routes)
	{
		for (FPlayerComboStep& Step : Route.Steps)
		{
			Step.InputToAttackText =FString::Printf(TEXT("%s -> %s"), *Step.Input.ToString(), *Step.Attack.ToString());
		}
	}
}
#endif

FPlayerComboPosition FPlayerAttackTransitionTable::FindContinuation(const FPlayerComboPosition& Current, const FGameplayTag& Input) const
{
	const int32 NextStepIndex = Current.IsValid() ? Current.StepIndex + 1 : 0;

	for (int32 RouteIndex = 0; RouteIndex < Routes.Num(); ++RouteIndex)
	{
		const FPlayerComboRoute& Route = Routes[RouteIndex];
		if (!Route.Steps.IsValidIndex(NextStepIndex) || Route.Steps[NextStepIndex].Input != Input)
		{
			continue;
		}

		// 지금까지의 입력 순서가 같은 루트만 이어진다.
		if (Current.IsValid() && !HasSameInputs(Route, Routes[Current.RouteIndex], NextStepIndex))
		{
			continue;
		}

		FPlayerComboPosition Next;
		Next.RouteIndex = RouteIndex;
		Next.StepIndex = NextStepIndex;
		return Next;
	}

	return FPlayerComboPosition();
}

bool FPlayerAttackTransitionTable::HasSameInputs(const FPlayerComboRoute& A, const FPlayerComboRoute& B, int32 StepCount) const
{
	if (A.Steps.Num() < StepCount || B.Steps.Num() < StepCount)
	{
		return false;
	}

	for (int32 StepIndex = 0; StepIndex < StepCount; ++StepIndex)
	{
		if (A.Steps[StepIndex].Input != B.Steps[StepIndex].Input)
		{
			return false;
		}
	}
	return true;
}
