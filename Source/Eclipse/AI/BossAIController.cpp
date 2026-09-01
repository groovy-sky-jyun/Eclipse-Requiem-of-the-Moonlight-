// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAIController.h"
#include "Eclipse.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BossAttack.h"
#include "EclipseGameMode.h"
#include "Kismet/GameplayStatics.h"


const FName ABossAIController::BB_SelectedAttack = TEXT("SelectedAttack");
const FName ABossAIController::BB_bCanReceiveDamage = TEXT("bCanReceiveDamage");
const FName ABossAIController::BB_ActiveWraithCount = TEXT("ActiveWraithCount");
const FName ABossAIController::BB_OrbitAngle = TEXT("OrbitAngle");
const FName ABossAIController::BB_bIsGroggy = TEXT("bIsGroggy");
const FName ABossAIController::BB_CurrentPhase = TEXT("CurrentPhase");
const FName ABossAIController::BB_bIsInCombat = TEXT("bIsInCombat");


ABossAIController::ABossAIController()
{

}

void ABossAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!BehaviorTreeAsset|| !BlackboardComponent)
	{
		UE_LOG(LogEclipse, Error, TEXT("BossAIController: BehaviorTreeAsset is NULL"));
		return;
	}

	BlackboardComponent->SetValueAsEnum(BB_SelectedAttack, static_cast<uint8>(EBossAttackType::None));
	BlackboardComponent->SetValueAsBool(BB_bCanReceiveDamage, true);
	BlackboardComponent->SetValueAsInt(BB_ActiveWraithCount, 0);
	BlackboardComponent->SetValueAsFloat(BB_OrbitAngle, 0.f);
	BlackboardComponent->SetValueAsBool(BB_bIsGroggy, false);
	BlackboardComponent->SetValueAsInt(BB_CurrentPhase, 1);
	BlackboardComponent->SetValueAsBool(BB_bIsDead, false);

	// 전투 개시는 GameMode가 정한다. 도중에 빙의해도 현재 상태를 그대로 따른다.
	AEclipseGameMode* GameMode = AEclipseGameMode::Get(this);
	BlackboardComponent->SetValueAsBool(BB_bIsInCombat, GameMode && GameMode->IsBattleActive());

	if (GameMode)
	{
		GameMode->OnBattleStarted.AddDynamic(this, &ABossAIController::HandleBattleStarted);
	}

	RunBehaviorTree(BehaviorTreeAsset);
}

void ABossAIController::OnUnPossess()
{
	if (AEclipseGameMode* GameMode = AEclipseGameMode::Get(this))
	{
		GameMode->OnBattleStarted.RemoveDynamic(this, &ABossAIController::HandleBattleStarted);
	}

	Super::OnUnPossess();
}

void ABossAIController::HandleBattleStarted(AEnemyBoss* InBoss)
{
	if (!BlackboardComponent) return;

	BlackboardComponent->SetValueAsBool(BB_bIsInCombat, true);

	UE_LOG(LogEclipse, Log, TEXT("[BossAI] Combat started"));
}
