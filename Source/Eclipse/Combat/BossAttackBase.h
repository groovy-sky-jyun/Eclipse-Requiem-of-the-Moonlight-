// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/TimerHandle.h"
#include "BossAttackBase.generated.h"

class AEnemyBoss;
class APawn;

/**
 * 보스 공격 하나를 표현하는 전략(Strategy) 객체.
 */
UCLASS(Abstract, EditInlineNew, DefaultToInstanced, Blueprintable, CollapseCategories)
class ECLIPSE_API UBossAttackBase : public UObject
{
	GENERATED_BODY()

public:
	// ── 실행 제어 (파생 클래스가 오버라이드하지 않는다) ─────────────

	void Begin(AEnemyBoss* InOwner);

	void Finish();

	void Cancel();

	void Tick(float DeltaTime);

	UFUNCTION(BlueprintPure, Category = "Boss|Attack")
	bool IsRunning() const { return bRunning; }

	virtual UWorld* GetWorld() const override;

protected:
	// ── 파생 클래스가 채우는 훅 ─────────────────────────────────

	virtual void OnStart() PURE_VIRTUAL(UBossAttackBase::OnStart, );

	virtual void OnTick(float DeltaTime) {}

	virtual void OnCancel() {}

	virtual void OnFinish() {}

	// ── 파생 클래스용 헬퍼 ──────────────────────────────────────

	AEnemyBoss* GetBoss() const { return Owner; }

	APawn* GetTargetPlayer() const;

	float GetElapsedTime() const;

	void SetAttackTimer(FTimerHandle& OutHandle, FTimerDelegate Delegate, float Delay, bool bLoop = false);

	void ClearAttackTimer(FTimerHandle& Handle);

protected:
	UPROPERTY(EditAnywhere, Category = "Attack|Safety", meta = (ClampMin = "0.5"))
	float MaxDuration = 12.f;

private:
	void OnWatchdogExpired();

	/** 다른 Attack의 예약된 모든 타이머 해제 */
	void ClearAllTimers();

private:
	UPROPERTY(Transient)
	TObjectPtr<AEnemyBoss> Owner = nullptr;

	bool bRunning = false;

	float StartTime = 0.f;

	FTimerHandle WatchdogHandle;

	TArray<FTimerHandle> ActiveTimers;
};
