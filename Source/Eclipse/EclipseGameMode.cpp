// Copyright Epic Games, Inc. All Rights Reserved.

#include "EclipseGameMode.h"
#include "Eclipse.h"
#include "EnemyBase.h"
#include "EnemyBoss.h"
#include "BossArena.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "Kismet/GameplayStatics.h"

AEclipseGameMode::AEclipseGameMode()
{
	PrimaryActorTick.bCanEverTick = false;
}

AEclipseGameMode* AEclipseGameMode::Get(const UObject* WorldContextObject)
{
	return Cast<AEclipseGameMode>(UGameplayStatics::GetGameMode(WorldContextObject));
}


// ── 등록 ─────────────────────────────────────────────────────────
void AEclipseGameMode::RegisterBoss(AEnemyBoss* InBoss)
{
	if (!IsValid(InBoss))
	{
		UE_LOG(LogEclipse, Warning, TEXT("[GameMode] RegisterBoss: 유효하지 않은 보스"));
		return;
	}

	if (Boss && Boss != InBoss)
	{
		UE_LOG(LogEclipse, Warning, TEXT("[GameMode] 보스가 이미 등록되어 있습니다. (%s → %s로 교체)"),
			*Boss->GetName(), *InBoss->GetName());
	}

	Boss = InBoss;
	UE_LOG(LogEclipse, Log, TEXT("[GameMode] 보스 등록: %s"), *InBoss->GetName());
}

void AEclipseGameMode::RegisterArena(ABossArena* InArena)
{
	if (!IsValid(InArena))
	{
		UE_LOG(LogEclipse, Warning, TEXT("[GameMode] RegisterArena: 유효하지 않은 아레나"));
		return;
	}

	if (Arena && Arena != InArena)
	{
		UE_LOG(LogEclipse, Warning, TEXT("[GameMode] 아레나가 이미 등록되어 있습니다. (%s → %s로 교체)"),
			*Arena->GetName(), *InArena->GetName());
	}

	Arena = InArena;
	UE_LOG(LogEclipse, Log, TEXT("[GameMode] 아레나 등록: %s"), *InArena->GetName());
}


// ── 전투 시작/종료 ─────────────────────────────────────────────────────────
void AEclipseGameMode::StartBattle()
{
	if (BattleResult != EBattleResult::NotStarted) return;

	BattleResult = EBattleResult::InProgress;

	UE_LOG(LogEclipse, Log, TEXT("[GameMode] 보스전 시작"));

	OnBattleStarted.Broadcast(Boss);
}

void AEclipseGameMode::NotifyBossDefeated()
{
	FinishBattle(EBattleResult::Victory);
}

void AEclipseGameMode::NotifyPlayerDied()
{
	FinishBattle(EBattleResult::Defeat);
}

void AEclipseGameMode::FinishBattle(EBattleResult Result)
{
	// 보스의 마지막 일격과 플레이어의 마지막 일격이 같은 프레임에 들어오는 경우,
	// 먼저 도착한 쪽이 결과를 확정한다.

	if (BattleResult == EBattleResult::Victory || BattleResult == EBattleResult::Defeat)
	{
		return;
	}

	BattleResult = Result;

	UE_LOG(LogEclipse, Log, TEXT("[GameMode] 보스전 종료: %s"),
		Result == EBattleResult::Victory ? TEXT("VICTORY") : TEXT("DEFEAT"));

	FreezeGameplay();

	// 결과 화면이 남은 시간을 읽으므로 알리기 전에 타이머를 건다.
	if (bAutoRestart)
	{
		GetWorldTimerManager().SetTimer(
			RestartTimerHandle,
			this,
			&AEclipseGameMode::RestartBattle,
			RestartDelay,
			false
		);
	}

	// UI / 연출은 블루프린트가 담당
	OnBattleFinished.Broadcast(Result);
}

void AEclipseGameMode::FreezeGameplay()
{
	// 1. 사망/클리어 연출 중 Input 차단
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		PC->DisableInput(PC);
	}

	// 2. 살아있는 모든 적 AI 정지.
	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsOfClass(this, AEnemyBase::StaticClass(), Enemies);

	for (AActor* EnemyActor : Enemies)
	{
		APawn* EnemyPawn = Cast<APawn>(EnemyActor);
		if (!IsValid(EnemyPawn)) continue;

		AAIController* EnemyAI = Cast<AAIController>(EnemyPawn->GetController());
		if (!EnemyAI) continue;

		if (UBrainComponent* Brain = EnemyAI->GetBrainComponent())
		{
			Brain->StopLogic(TEXT("Battle Finished"));
		}
	}
}

void AEclipseGameMode::RequestRestart()
{
	GetWorldTimerManager().ClearTimer(RestartTimerHandle);

	RestartBattle();
}

float AEclipseGameMode::GetRestartTimeRemaining() const
{
	return GetWorldTimerManager().GetTimerRemaining(RestartTimerHandle);
}

void AEclipseGameMode::RestartBattle()
{
	UE_LOG(LogEclipse, Log, TEXT("[GameMode] 레벨 재시작"));

	// 레벨 리로드 시 기존 pc가 삭제되어 DisableInput 설정도 초기화된다.
	const FString CurrentLevel = UGameplayStatics::GetCurrentLevelName(this, true);
	UGameplayStatics::OpenLevel(this, FName(*CurrentLevel));
}
