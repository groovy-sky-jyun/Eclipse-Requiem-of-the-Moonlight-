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

void AEnemyBase::HandleTakeDamage_Implementation(float DamageAmount, AActor* Attacker)
{
	Super::HandleTakeDamage_Implementation(DamageAmount, Attacker);
}

void AEnemyBase::Die_Implementation()
{
	Super::Die_Implementation();

	// AI 컨트롤러 정지 (죽은 몬스터가 계속 생각하지 않도록)
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		// 비헤이비어 트리나 브레인 컴포넌트를 멈추는 로직을 나중에 여기에 추가
		if (UBrainComponent* Brain = AIController->GetBrainComponent())
		{
			Brain->StopLogic("Enemy is Dead");
		}
	}
}
