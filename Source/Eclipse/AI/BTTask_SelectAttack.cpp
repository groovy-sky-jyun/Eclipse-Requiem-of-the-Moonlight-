// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SelectAttack.h"
#include "BossAIController.h"
#include "EnemyBoss.h"
#include "BossAttackComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SelectAttack::UBTTask_SelectAttack()
{
	NodeName = TEXT("Select Attack");
	bNotifyTick = true;
}

TArray<UBTTask_SelectAttack::FAttackEntry> UBTTask_SelectAttack::GetAttackPool(int32 Phase) const
{

	switch (Phase) // {공격, 가중치, 쿨타임(초), 거리조건 유무, 거리 Min, 거리 Max} // 가중치 합계는 꼭 100일 필요 x 페이즈별로 달라도 o
	{
	case 1:
		return {
			{ EBossAttackType::BloodBolt, 30.f, 5.f, true, 0.f, 800.f },
			{ EBossAttackType::DarkSweep, 50.f, 10.f, false }
		};
	case 2:
		return {
			{ EBossAttackType::BloodBolt, 20.f, 4.f, true, 0.f, 800.f },
			{ EBossAttackType::ShadowCrash, 50.f, 8.f, false },
			{ EBossAttackType::WraithDrop, 60.f, 12.f, true, 0.f, 1500.f },
			{ EBossAttackType::DarkSweep, 120.f, 25.f, false }
		};
	case 3:
	default:
		return {
			{ EBossAttackType::BloodBolt, 15.f, 3.f, true, 0.f, 800.f },
			{ EBossAttackType::ShadowCrash, 40.f, 6.f, false },
			{ EBossAttackType::WraithDrop, 45.f, 9.f, true, 0.f, 1500.f },
			{ EBossAttackType::DarkSweep, 80.f, 23.f, false },
			{ EBossAttackType::LunarBeam, 120.f, 34.f, true, 300.f, 600.f },
			{ EBossAttackType::EclipseVeil, 240.f, 50.f, false }
		};
	}
}

EBTNodeResult::Type UBTTask_SelectAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AEnemyBoss* Boss = Cast<AEnemyBoss>(OwnerComp.GetAIOwner()->GetPawn());
	APawn* Player = Cast<APawn>(BB->GetValueAsObject(ABossAIController::BB_TargetActor));
	if (!BB || !Boss || !Player) return EBTNodeResult::Failed;

	int32 Phase = BB->GetValueAsInt(ABossAIController::BB_CurrentPhase);
	float Now = GetWorld()->GetTimeSeconds();
	TArray<FAttackEntry> Pool = GetAttackPool(Phase);

	EBossAttackType Selected = PickAttack(Boss,Player,Pool, Boss->GetAttackComponent()->AttackLastUsedList, Now);
	if (Selected != EBossAttackType::None)
	{
		BB->SetValueAsEnum(ABossAIController::BB_SelectedAttack, (uint8)Selected);
		UE_LOG(LogTemp, Warning, TEXT("[SelectAttack] Phase%d / Boss Attack : %s"), Phase, *UEnum::GetValueAsString(Selected));
	}
		
	return EBTNodeResult::Succeeded;
}

EBossAttackType UBTTask_SelectAttack::PickAttack(AEnemyBoss* Boss, APawn* Player, const TArray<FAttackEntry>& Pool, const TMap<EBossAttackType, float>& LastUsed, float Now) const
{
	if (!Boss || !Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SelectAttack] is Failed : line66"));
		return EBossAttackType::None;
	}

	TArray<FAttackEntry> AvailablePool;
	for (const FAttackEntry& Entry : Pool)
	{
		// 1. Check CoolTime
		const float* LastTime = LastUsed.Find(Entry.Attack);
		float Elapsed = LastTime ? (Now - *LastTime) : 9999.f;
		
		if (Elapsed < Entry.Cooldown) continue;

		// 2. Check Distance
		if(!Entry.bHasCondition) AvailablePool.Add(Entry);
		else
		{
			float Dist2D = FVector::Dist2D(Boss->GetActorLocation(), Player->GetActorLocation());
			if (Dist2D >= Entry.MinDist && Dist2D <= Entry.MaxDist)
			{
				AvailablePool.Add(Entry);
			}
		}
	}

	if (AvailablePool.IsEmpty()) return EBossAttackType::None;

	// 2. 가중치 합산 후 랜덤 값으로 선택
	float TotalWeight = 0.f;
	for (const FAttackEntry& Entry : AvailablePool)
	{
		TotalWeight += Entry.Weight;
	}

	float Rand = FMath::FRandRange(0.f, TotalWeight);
	float Cumulative = 0.f;

	for (const FAttackEntry& Entry : AvailablePool)
	{
		Cumulative += Entry.Weight;
		if (Rand <= Cumulative)
		{
			return Entry.Attack;
		}
	}

	return AvailablePool.Last().Attack;
}

EBTNodeResult::Type UBTTask_SelectAttack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Aborted;
}