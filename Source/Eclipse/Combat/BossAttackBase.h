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
	virtual void OnStart() PURE_VIRTUAL(UBossAttackBase::OnStart, );

	virtual void OnTick(float DeltaTime) {}

	virtual void OnCancel() {}

	virtual void OnFinish() {}

// ── 파생 클래스용 헬퍼 ──────────────────────────────────────
	void SetAttackState(EBossAttackState NewState);

	AEnemyBoss* GetBoss() const { return Owner; }

	APawn* GetTargetPlayer() const;

	float GetElapsedTime() const;

	void SetAttackTimer(FTimerHandle& OutHandle, FTimerDelegate Delegate, float Delay, bool bLoop = false);

	void ClearAttackTimer(FTimerHandle& Handle);

protected:
	UPROPERTY(EditAnywhere, Category = "Attack|Safety", meta = (ClampMin = "0.5"))
	float MaxDuration = 2.f;

private:
	void OnWatchdogExpired();

	/** 다른 Attack의 예약된 모든 타이머 해제 */
	void ClearAllTimers();

private:
	UPROPERTY(Transient)
	TObjectPtr<AEnemyBoss> Owner = nullptr;

	EBossAttackState AttackState = EBossAttackState::Idle;

	float StartTime = 0.f;

	FTimerHandle WatchdogHandle;

	TArray<FTimerHandle> ActiveTimers;
};
