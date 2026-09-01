// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAttackComponent.h"
#include "Eclipse.h"
#include "EnemyBoss.h"
#include "BossAttackBase.h"
#include "Engine/DataTable.h"

UBossAttackComponent::UBossAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBossAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	Boss = Cast<AEnemyBoss>(GetOwner());
	if (!Boss)
	{
		UE_LOG(LogEclipse, Error, TEXT("[BossAttack] Owner is not AEnemyBoss"));
	}

	CacheAttackPool();
}


void UBossAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsValid(CurrentAttack))
	{
		CurrentAttack->Tick(DeltaTime);
	}
}

void UBossAttackComponent::CacheAttackPool()
{
	PoolCacheByPhase.Empty();

	if (!AttackPoolTable)
	{
		UE_LOG(LogEclipse, Error, TEXT("[BossAttack] AttackPoolTable is Empty"));
		return;
	}

	static const FString Context(TEXT("BossAttackPool"));
	TArray<FBossAttackPoolRow*> Rows;
	AttackPoolTable->GetAllRows(Context, Rows);

	for (const FBossAttackPoolRow* Row : Rows)
	{
		if (!Row) continue;

		// TSoftClassPtr는 여기서 한 번만 로드한다. 고를 때마다 로드하면 프레임이 튄다.
		if (!Row->AttackClass.LoadSynchronous())
		{
			UE_LOG(LogEclipse, Warning, TEXT("[BossAttack] Phase %d row has empty AttackClass. Skipped"), Row->Phase);
			continue;
		}

		PoolCacheByPhase.FindOrAdd(Row->Phase).Add(*Row);
	}
}


TSubclassOf<UBossAttackBase> UBossAttackComponent::SelectAttack(int32 Phase, APawn* Target)
{
	PendingAttackClass = nullptr;

	if (!IsValid(Boss)) return nullptr;

	const TArray<FBossAttackPoolRow>* PoolByPhase = PoolCacheByPhase.Find(Phase);
	if (!PoolByPhase || PoolByPhase->IsEmpty())
	{
		UE_LOG(LogEclipse, Warning, TEXT("[BossAttack] Phase %d Attack Pool is Empty"), Phase);
		return nullptr;
	}

	const float Now = GetWorld()->GetTimeSeconds();
	const float Dist2D = Target ? FVector::Dist2D(Boss->GetActorLocation(), Target->GetActorLocation()) : 0.f;

	// 1) 쿨타임 / 거리로 후보를 거른다.
	TArray<const FBossAttackPoolRow*> Available;
	float TotalWeight = 0.f;

	for (const FBossAttackPoolRow& Row : *PoolByPhase)
	{
		const TSubclassOf<UBossAttackBase> RowClass = Row.AttackClass.Get();
		if (!RowClass || Row.Weight <= 0.f) continue;

		if (const float* LastUsed = LastUsedTimeList.Find(RowClass))
		{
			if ((Now - *LastUsed) < Row.Cooldown) continue;
		}

		if (Row.bUseDistanceCondition)
		{
			if (!Target) continue;
			if (Dist2D < Row.MinDistance || Dist2D > Row.MaxDistance) continue;
		}

		Available.Add(&Row);
		TotalWeight += Row.Weight;
	}

	if (Available.IsEmpty() || TotalWeight <= 0.f) return nullptr;

	// 2) 가중치 룰렛
	float Rand = FMath::FRandRange(0.f, TotalWeight);
	for (const FBossAttackPoolRow* Row : Available)
	{
		Rand -= Row->Weight;
		if (Rand <= 0.f)
		{
			PendingAttackClass = Row->AttackClass.Get();
			break;
		}
	}

	if (!PendingAttackClass)
	{
		PendingAttackClass = Available.Last()->AttackClass.Get();
	}

	return PendingAttackClass;
}


void UBossAttackComponent::ExecuteAttack()
{
	if (!IsValid(Boss) || !PendingAttackClass) return;

	LastUsedTimeList.Add(PendingAttackClass, GetWorld()->GetTimeSeconds());

	if (IsValid(CurrentAttack) && CurrentAttack->IsRunning()) // 이전 공격의 종료 통보가 누락된 상태
	{
		UE_LOG(LogEclipse, Warning, TEXT("[BossAttack] Previous attack still running : %s. Force cancel."),
			*CurrentAttack->GetClass()->GetName());

		CurrentAttack->Cancel(); // 예약한 타이머 전부 제거
	}

	CurrentAttack = NewObject<UBossAttackBase>(this, PendingAttackClass);
	CurrentAttack->Begin(Boss);

	// 예약은 한 번만 소비
	PendingAttackClass = nullptr;
}

EBossAttackState UBossAttackComponent::GetAttackState() const
{
	return IsValid(CurrentAttack) ? CurrentAttack->GetAttackState() : EBossAttackState::Idle;
}

bool UBossAttackComponent::TryCancelStartupAttack()
{
	if (GetAttackState() != EBossAttackState::Startup) return false;

	CurrentAttack->Cancel();
	return true;
}

void UBossAttackComponent::CancelCurrent()
{
	// BTTask가 Aborted로 끝날 때 & 보스 사망 시 호출
	if (IsValid(CurrentAttack))
	{
		CurrentAttack->Cancel();
	}
}

void UBossAttackComponent::NotifyAttackStateChanged(EBossAttackState NewState)
{
	OnAttackStateChangedDelegate.Broadcast(NewState);
}

void UBossAttackComponent::NotifyAttackFinished()
{
	if (OnAttackFinishedDelegate.IsBound())
	{
		OnAttackFinishedDelegate.Broadcast();
		UE_LOG(LogEclipse, Verbose, TEXT("Attack Finished Broadcast"));
	}
}
