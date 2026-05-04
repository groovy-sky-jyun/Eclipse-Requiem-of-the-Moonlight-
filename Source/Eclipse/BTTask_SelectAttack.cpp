// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SelectAttack.h"
#include "BossAIController.h"
#include "EnemyBoss.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SelectAttack::UBTTask_SelectAttack()
{
	NodeName = TEXT("Select Attack");
}

EBTNodeResult::Type UBTTask_SelectAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AEnemyBoss* Boss = Cast<AEnemyBoss>(OwnerComp.GetAIOwner()->GetPawn());
	if (!BB || !Boss) return EBTNodeResult::Failed;

	int32 Phase = BB->GetValueAsInt(ABossAIController::BB_CurrentPhase);
	float Now = GetWorld()->GetTimeSeconds();

	TArray<FAttackEntry> Pool = GetAttackPool(Phase);
	EBossAttackType Selected = PickAttack(Pool, Boss->AttackLastUsedTime, Now);

	BB->SetValueAsEnum(ABossAIController::BB_SelectedAttack, (uint8)Selected);

	UE_LOG(LogTemp, Warning, TEXT("Phase %d → %s"), Phase, *UEnum::GetValueAsString(Selected));

	return EBTNodeResult::Succeeded;
}

TArray<UBTTask_SelectAttack::FAttackEntry> UBTTask_SelectAttack::GetAttackPool(int32 Phase) const
{

	switch (Phase) // {공격, 가중치, 쿨타임(초)} // 가중치 합계는 꼭 100일 필요 x 페이즈별로 달라도 o
	{
	case 1:
		return {
			{ EBossAttackType::BloodBolt,     60.f,  3.f  },
			{ EBossAttackType::DamningTether, 30.f,  18.f },
			{ EBossAttackType::MiasmaStep,    10.f,  30.f },
		};
	case 2:
		return {
			{ EBossAttackType::BloodBolt,     30.f,  4.f  },
			{ EBossAttackType::WraithDrop,    35.f,  25.f },
			{ EBossAttackType::LunarBeam,     20.f,  30.f },
			{ EBossAttackType::DamningTether, 15.f,  14.f },
			// UltimateAttack은 HP 35% 강제 트리거이므로 풀에서 제외
		};
	case 3:
	default:
		return {
			{ EBossAttackType::BloodBolt,     25.f,  3.f  },
			{ EBossAttackType::WraithDrop,    30.f,  20.f },
			{ EBossAttackType::LunarBeam,     25.f,  18.f },
			{ EBossAttackType::DamningTether, 20.f,  12.f },
			// UltimateAttack은 BTTask_UltimateAttack이 별도 처리
		};
	}
}

EBossAttackType UBTTask_SelectAttack::PickAttack(const TArray<FAttackEntry>& Pool, const TMap<EBossAttackType, float>& LastUsed, float Now) const
{
	TArray<FAttackEntry> Available;
	for (const FAttackEntry& Entry : Pool)
	{
		// 1. Only attacks after cool time are included in the Available Attack List
		const float* LastTime = LastUsed.Find(Entry.Attack);
		float Elapsed = LastTime ? (Now - *LastTime) : 9999.f;

		if (Elapsed >= Entry.Cooldown)
		{
			Available.Add(Entry);
		}
	}

	if (Available.IsEmpty()) return EBossAttackType::None;

	// 2. 가중치 합산 후 랜덤 값으로 선택
	float TotalWeight = 0.f;
	for (const FAttackEntry& Entry : Available)
	{
		TotalWeight += Entry.Weight;
	}

	float Rand = FMath::FRandRange(0.f, TotalWeight);
	float Cumulative = 0.f;

	for (const FAttackEntry& Entry : Available)
	{
		Cumulative += Entry.Weight;
		if (Rand <= Cumulative)
		{
			return Entry.Attack;
		}
	}

	return Available.Last().Attack;
}
