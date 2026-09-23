// Fill out your copyright notice in the Description page of Project Settings.

#include "StaminaBarWidget.h"
#include "PlayerCharacter.h"
#include "StaminaComponent.h"

void UStaminaBarWidget::BindPlayer(APlayerCharacter* NewTarget)
{
	UnbindTarget();

	if (!IsValid(NewTarget))
	{
		OnStaminaUpdated(0.f, 0.f, 0.f);
		return;
	}

	UStaminaComponent* StaminaComp = NewTarget->GetStaminaComponent();
	if (!StaminaComp)
	{
		OnStaminaUpdated(0.f, 0.f, 0.f);
		return;
	}

	Target = NewTarget;
	StaminaComp->OnStaminaChangedDelegate.AddUObject(this, &UStaminaBarWidget::HandleStaminaChanged);

	HandleStaminaChanged(StaminaComp->GetCurrentStamina(), StaminaComp->GetMaxStamina());
}

void UStaminaBarWidget::NativeDestruct()
{
	UnbindTarget();

	Super::NativeDestruct();
}

void UStaminaBarWidget::HandleStaminaChanged(float Current, float Max)
{
	const float Ratio = (Max > 0.f) ? FMath::Clamp(Current / Max, 0.f, 1.f) : 0.f;

	OnStaminaUpdated(Ratio, Current, Max);
}

void UStaminaBarWidget::UnbindTarget()
{
	if (APlayerCharacter* OldTarget = Target.Get())
	{
		if (UStaminaComponent* StaminaComp = OldTarget->GetStaminaComponent())
		{
			StaminaComp->OnStaminaChangedDelegate.RemoveAll(this);
		}
	}

	Target = nullptr;
}
