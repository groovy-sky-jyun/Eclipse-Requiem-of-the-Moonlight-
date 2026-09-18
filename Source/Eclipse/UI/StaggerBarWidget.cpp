// Fill out your copyright notice in the Description page of Project Settings.

#include "StaggerBarWidget.h"
#include "EnemyBoss.h"
#include "BossGroggyComponent.h"

void UStaggerBarWidget::BindBoss(AEnemyBoss* NewTarget)
{
	UnbindTarget();

	if (!IsValid(NewTarget))
	{
		OnStaggerUpdated(0.f, 0.f, 0.f);
		return;
	}

	UBossGroggyComponent* GroggyComp = NewTarget->GetGroggyComponent();
	if (!GroggyComp)
	{
		OnStaggerUpdated(0.f, 0.f, 0.f);
		return;
	}

	Target = NewTarget;
	GroggyComp->OnStaggerChangedDelegate.AddUObject(this, &UStaggerBarWidget::HandleStaggerChanged);

	// 델리게이트는 값이 바뀔 때만 온다. 지금 게이지로 한 번 채워둔다.
	HandleStaggerChanged(GroggyComp->GetCurrentStagger(), GroggyComp->GetStaggerThreshold());
}

void UStaggerBarWidget::NativeDestruct()
{
	UnbindTarget();

	Super::NativeDestruct();
}

void UStaggerBarWidget::HandleStaggerChanged(float Current, float Max)
{
	const float Ratio = (Max > 0.f) ? FMath::Clamp(Current / Max, 0.f, 1.f) : 0.f;

	OnStaggerUpdated(Ratio, Current, Max);
}

void UStaggerBarWidget::UnbindTarget()
{
	if (AEnemyBoss* OldTarget = Target.Get())
	{
		if (UBossGroggyComponent* GroggyComp = OldTarget->GetGroggyComponent())
		{
			GroggyComp->OnStaggerChangedDelegate.RemoveAll(this);
		}
	}

	Target = nullptr;
}
