// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthBarWidget.h"
#include "BaseCharacter.h"

void UHealthBarWidget::BindCharacter(ABaseCharacter* NewTarget)
{
	UnbindTarget();

	if (!IsValid(NewTarget))
	{
		OnHealthUpdated(0.f, 0.f, 0.f);
		return;
	}

	Target = NewTarget;
	NewTarget->OnHealthChangedDelegate.AddDynamic(this, &UHealthBarWidget::HandleHealthChanged);

	// 델리게이트는 값이 바뀔 때만 온다. 지금 체력으로 한 번 채워둔다.
	HandleHealthChanged(ICombatInterface::Execute_GetHealth(NewTarget), ICombatInterface::Execute_GetMaxHealth(NewTarget));
}

void UHealthBarWidget::NativeDestruct()
{
	UnbindTarget();

	Super::NativeDestruct();
}

void UHealthBarWidget::HandleHealthChanged(float Current, float Max)
{
	const float Ratio = (Max > 0.f) ? FMath::Clamp(Current / Max, 0.f, 1.f) : 0.f;

	OnHealthUpdated(Ratio, Current, Max);
}

void UHealthBarWidget::UnbindTarget()
{
	if (ABaseCharacter* OldTarget = Target.Get())
	{
		OldTarget->OnHealthChangedDelegate.RemoveDynamic(this, &UHealthBarWidget::HandleHealthChanged);
	}

	Target = nullptr;
}
