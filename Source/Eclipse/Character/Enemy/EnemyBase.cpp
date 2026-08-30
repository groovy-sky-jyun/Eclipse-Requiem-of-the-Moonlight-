// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"
#include "Components/WidgetComponent.h"
#include "AIController.h"
#include "BrainComponent.h"

AEnemyBase::AEnemyBase()
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AEnemyBase::OnDeath()
{
	if (BB)
	{
		BB->SetValueAsBool(ABaseEnemyAIController::BB_bIsDead, true);
	}

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		if (UBrainComponent* Brain = AIController->GetBrainComponent())
		{
			Brain->StopLogic("Enemy is Dead");
		}
	}
}

