// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_UpdatePhase.h"
#include "BossAIController.h"
#include "EnemyBoss.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_UpdatePhase::UBTService_UpdatePhase()
{
	NodeName = TEXT("Update Phase");
	Interval = 0.5f;
	RandomDeviation = 0.1f;
}

void UBTService_UpdatePhase::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AEnemyBoss* Boss = Cast<AEnemyBoss>(OwnerComp.GetAIOwner()->GetPawn());
	if (!BB || !Boss) return;

	//Prevent Division by Zero
	float HPRatio = Boss->GetHealth() / FMath::Max(Boss->GetMaxHealth(), 1.0f);

	int32 NewPhase = 1;
	if (HPRatio <= 4.5f) NewPhase = 3;
	else if (HPRatio <= 7.f) NewPhase = 2;

	if (NewPhase != LastKnownPhase)
	{
		LastKnownPhase = NewPhase;
		BB->SetValueAsInt(ABossAIController::BB_CurrentPhase, NewPhase);

		//Boss->EnterPhase(NewPhase);
	}
}
