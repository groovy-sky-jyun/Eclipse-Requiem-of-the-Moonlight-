// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBoss.h"
#include "Eclipse.h"
#include "EnemyMinion.h"
#include "BossAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EclipseGameMode.h"
#include "BossPhaseComponent.h"
#include "BossAttackComponent.h"

AEnemyBoss::AEnemyBoss()
{
	PrimaryActorTick.bCanEverTick = true;

	MaxHealth = 1500.f;

	PhaseComponent = CreateDefaultSubobject<UBossPhaseComponent>(TEXT("PhaseComponent"));
	AttackComponent = CreateDefaultSubobject<UBossAttackComponent>(TEXT("AttackComponent"));

}

void AEnemyBoss::BeginPlay()
{
	Super::BeginPlay();

	if (AEclipseGameMode* GameMode = AEclipseGameMode::Get(this))
	{
		GameMode->RegisterBoss(this);
	}

	AI = Cast<ABossAIController>(GetController());
	if (!AI)
	{
		UE_LOG(LogEclipse, Error, TEXT("[Boss] AI Controller is NULL"));
		return;
	}

	BB = AI->GetBlackboardComponent();
	if (!BB)
	{
		UE_LOG(LogEclipse, Error, TEXT("[AIController] Blackboard is NULL"));
		return;
	}
}


// ── 데미지 / 사망 ─────────────────────────────────────────────
void AEnemyBoss::OnDamaged(float DamageAmount, AActor* Attacker, bool bLethal)
{
	if (bLethal) return;

	if (PhaseComponent)
	{
		PhaseComponent->AddStaggerDamage(DamageAmount);
	}
}

void AEnemyBoss::OnDeath()
{
	// 진행 중인 공격이 죽은 뒤에도 타이머를 돌리지 않도록 끊는다.
	if (AttackComponent)
	{
		AttackComponent->CancelCurrent();
	}

	Super::OnDeath();

	if (AEclipseGameMode* GameMode = AEclipseGameMode::Get(this))
	{
		GameMode->NotifyBossDefeated();
	}
}


// ── 플레이어 궁극기 -> 그로기 캔슬 시도ㅠ ─────────────────────────────────────────────
bool AEnemyBoss::TryGroggyByUltimate()
{
	if (!AttackComponent || !PhaseComponent) return false;

	// 예열이 아니면 실패한다. 스태거 누적은 피격 경로가 알아서 한다.
	if (!AttackComponent->TryCancelWindupAttack()) return false;

	PhaseComponent->TriggerGroggy();
	return true;
}


// ── 페이즈 ─────────────────────────────────────────────
int32 AEnemyBoss::GetCurrentPhase() const
{
	return PhaseComponent ? PhaseComponent->GetCurrentPhase() : 1;
}

void AEnemyBoss::EnterPhase(int32 NewPhase)
{
	if (PhaseComponent)
	{
		PhaseComponent->EnterPhase(NewPhase);
	}
}


// ── 망령 카운트 ───────────────────────────────────────────────
void AEnemyBoss::OnWraithSpawned(AEnemyMinion* Minion)
{
	if (!IsValid(Minion)) return;

	Minion->OwnerBoss = this;
	ActiveWraithCount++;

	if (BB)
	{
		BB->SetValueAsInt(ABossAIController::BB_ActiveWraithCount, ActiveWraithCount);
	}
}

void AEnemyBoss::OnWraithDied()
{
	ActiveWraithCount = FMath::Max(0, ActiveWraithCount - 1);

	AI->GetBlackboardComponent()->SetValueAsInt(ABossAIController::BB_ActiveWraithCount, ActiveWraithCount);
	UE_LOG(LogEclipse, Log, TEXT("Wraith Die. Left Wraith is %d"), ActiveWraithCount);
}
