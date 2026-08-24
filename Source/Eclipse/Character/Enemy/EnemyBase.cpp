// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"
#include "Components/WidgetComponent.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BrainComponent.h"

AEnemyBase::AEnemyBase()
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AEnemyBase::HandleDeath()
{
	Super::HandleDeath();

	// 아래 StopLogic보다 반드시 먼저. 순서가 바뀌면 트리가 이미 멈춘 뒤라 의미가 없다.
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
