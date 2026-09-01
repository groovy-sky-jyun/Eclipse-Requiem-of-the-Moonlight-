// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/TimerHandle.h"
#include "BossAttackBase.generated.h"

class AEnemyBoss;
class APawn;

/** 공격 진행 단계. Idle이 아니면 실행 중이다. */
UENUM(BlueprintType)
enum class EBossAttackState : uint8
{
	Idle,
	Windup,
	Active,
	Recovery
};

/**
 * 보스 공격 객체.
 */
UCLASS(Abstract, EditInlineNew, DefaultToInstanced, Blueprintable, CollapseCategories)
class ECLIPSE_API UBossAttackBase : public UObject
{
	GENERATED_BODY()

// ── 실행 제어 (파생 클래스가 오버라이드하지 않는다) ─────────────
public:
	void Begin(AEnemyBoss* InOwner);

	void Tick(float DeltaTime);

	void Cancel();

	void Finish();

	UFUNCTION(BlueprintPure, Category = "Boss|Attack")
	bool IsRunning() const { return AttackState != EBossAttackState::Idle; }

	UFUNCTION(BlueprintPure, Category = "Boss|Attack")
	EBossAttackState GetAttackState() const { return AttackState; }

	virtual UWorld* GetWorld() const override;


// ── 파생 클래스가 채우는 훅 ─────────────────────────────────
protected:
	virtual void OnWindup() PURE_VIRTUAL(UBossAttackBase::OnWindup, );

	/** 예열이 끝나 판정이 열리는 시점. WindupTime이 0이면 호출되지 않는다. */
	virtual void OnActive() {}

	/** 판정이 끝나고 후딜에 들어간 시점. */
	virtual void OnRecovery() {}

	virtual void OnTick(float DeltaTime) {}

	virtual void OnCancel() {}

	virtual void OnFinish() {}

// ── 파생 클래스용 헬퍼 ──────────────────────────────────────
	/** 판정을 연다. WindupTime이 지나면 베이스가 부르지만, 앞당겨 끊을 수도 있다. */
	void EnterActive();

	/** 후딜에 들어간다. RecoveryTime이 지나면 스스로 Finish한다. */
	void EnterRecovery();

	AEnemyBoss* GetBoss() const { return Owner; }

	APawn* GetTargetPlayer() const;

	float GetElapsedTime() const;

	void SetAttackTimer(FTimerHandle& OutHandle, FTimerDelegate Delegate, float Delay, bool bLoop = false);

	void ClearAttackTimer(FTimerHandle& Handle);

protected:
	/** 예열 시간. 0이면 베이스가 관여하지 않고 파생이 직접 상태를 전환한다. */
	UPROPERTY(EditAnywhere, Category = "Attack|Timing", meta = (ClampMin = "0.0"))
	float WindupTime = 0.f;

	/** 후딜 시간. EnterRecovery부터 Finish까지의 간격이다. */
	UPROPERTY(EditAnywhere, Category = "Attack|Timing", meta = (ClampMin = "0.0"))
	float RecoveryTime = 0.f;

	UPROPERTY(EditAnywhere, Category = "Attack|Safety", meta = (ClampMin = "0.5"))
	float MaxDuration = 2.f;

private:
	/** 상태 전환은 베이스만 한다. 파생은 EnterActive / EnterRecovery로 요청한다. */
	void SetAttackState(EBossAttackState NewState);

	void OnWatchdogExpired();

	/** 다른 Attack의 예약된 모든 타이머 해제 */
	void ClearAllTimers();

private:
	UPROPERTY(Transient)
	TObjectPtr<AEnemyBoss> Owner = nullptr;

	EBossAttackState AttackState = EBossAttackState::Idle;

	float StartTime = 0.f;

	FTimerHandle WatchdogHandle;

	/** Windup -> Active, Recovery -> Finish 전환에 쓰는 타이머 */
	FTimerHandle PatternHandle;

	TArray<FTimerHandle> ActiveTimers;
};
