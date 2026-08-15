// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EclipseGameMode.generated.h"

class AEnemyBoss;
class ABossArena;

/** 보스전 진행 상태 */
UENUM(BlueprintType)
enum class EBattleResult : uint8
{
	NotStarted  UMETA(DisplayName = "Not Started"),
	InProgress  UMETA(DisplayName = "In Progress"),
	Victory     UMETA(DisplayName = "Victory"),
	Defeat      UMETA(DisplayName = "Defeat")
};

/** 전투 시작 알림. 보스 체력바 UI 표시 등에 사용 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBattleStarted, AEnemyBoss*, Boss);

/** 전투 종료 알림. 승패 UI / 연출 / 사운드에 사용 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBattleFinished, EBattleResult, Result);

/**
 *  게임의 시작 / 승리 / 패배 / 재시작 관리.
 */
UCLASS(abstract)
class AEclipseGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	/** Constructor */
	AEclipseGameMode();

	/** 현재 월드의 EclipseGameMode를 가져온다. (싱글플레이 전용) */
	UFUNCTION(BlueprintPure, Category = "Battle", meta = (WorldContext = "WorldContextObject"))
	static AEclipseGameMode* Get(const UObject* WorldContextObject);


// ── 등록 ─────────────────────────────────────────────────
public:
	/** 보스가 BeginPlay에서 자신을 등록한다. */
	void RegisterBoss(AEnemyBoss* InBoss);

	UFUNCTION(BlueprintPure, Category = "Battle")
	AEnemyBoss* GetBoss() const { return Boss; }

	/** 아레나가 BeginPlay에서 자신을 등록한다. */
	void RegisterArena(ABossArena* InArena);

	/** 배치된 아레나. 없을 수도 있다(테스트 레벨 등). */
	UFUNCTION(BlueprintPure, Category = "Battle")
	ABossArena* GetArena() const { return Arena; }


// ── 전투 흐름 ─────────────────────────────────────────────
public:
	/** 플레이어가 아레나에 진입했을 때. 중복 호출은 무시된다. */
	UFUNCTION(BlueprintCallable, Category = "Battle")
	void StartBattle();

	/** 보스 사망 → 승리 */
	UFUNCTION(BlueprintCallable, Category = "Battle")
	void NotifyBossDefeated();

	/** 플레이어 사망 → 패배 */
	UFUNCTION(BlueprintCallable, Category = "Battle")
	void NotifyPlayerDied();

	UFUNCTION(BlueprintPure, Category = "Battle")
	EBattleResult GetBattleResult() const { return BattleResult; }

	UFUNCTION(BlueprintPure, Category = "Battle")
	bool IsBattleActive() const { return BattleResult == EBattleResult::InProgress; }


// ── 블루프린트 훅 ──────────────────────────────────────────
public:
	UPROPERTY(BlueprintAssignable, Category = "Battle")
	FOnBattleStarted OnBattleStarted;

	UPROPERTY(BlueprintAssignable, Category = "Battle")
	FOnBattleFinished OnBattleFinished;


protected:
	/** 승리 / 패배 공통 종료 처리 */
	void FinishBattle(EBattleResult Result);

	/** 플레이어 입력 차단 + 살아있는 모든 적 AI 정지 */
	void FreezeGameplay();

	/** RestartDelay 경과 후 호출 */
	void RestartBattle();


// ── 설정 ─────────────────────────────────────────────────
protected:
	/** 전투 종료 후 자동으로 레벨을 재시작할지 여부 */
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Battle")
	bool bAutoRestart = true;

	/** 전투 종료부터 재시작까지의 대기 시간(초) */
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Battle",
		meta = (EditCondition = "bAutoRestart", ClampMin = "0.0"))
	float RestartDelay = 4.f;


// ── 런타임 상태 ────────────────────────────────────────────
protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Battle")
	EBattleResult BattleResult = EBattleResult::NotStarted;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Battle")
	TObjectPtr<AEnemyBoss> Boss;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Battle")
	TObjectPtr<ABossArena> Arena;


private:
	FTimerHandle RestartTimerHandle;
};
