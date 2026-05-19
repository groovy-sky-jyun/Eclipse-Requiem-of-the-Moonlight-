// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBoss.h"
#include "EnemyMinion.h"
#include "BossAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"

AEnemyBoss::AEnemyBoss()
{
	PrimaryActorTick.bCanEverTick = true;

	MaxHealth = 1500.f;
	CurrentHealth = MaxHealth;
	CurrentPhase = 1;

}

void AEnemyBoss::BeginPlay()
{
	Super::BeginPlay();

	AI = Cast<ABossAIController>(GetController());
	if (!AI)
	{
		UE_LOG(LogTemp, Error, TEXT("[Boss] AI Controller is NULL"));
		return;
	}

	BB = AI->GetBlackboardComponent();
	if (!BB)
	{
		UE_LOG(LogTemp, Error, TEXT("[AIController] Blackboard is NULL"));
		return;
	}

	
	// 공중에서 시작
	//SetFlying(true);
}


// ── 데미지 / 사망 ─────────────────────────────────────────────
void AEnemyBoss::HandleTakeDamage_Implementation(float DamageAmount, AActor* Attacker)
{
	if (!CanBeDamaged()) return;

	Super::HandleTakeDamage_Implementation(DamageAmount, Attacker);

	if (!AI || !BB) return;

	float Now = GetWorld()->GetTimeSeconds();
	if ((Now - TimeSinceLastHit) > StaggerResetTime) //StaggerResetTime 안에 다음 피격이 들어와야함.
	{
		StaggerAccumulated = 0.f;
	}
	TimeSinceLastHit = Now;

	StaggerAccumulated += DamageAmount;

	if (StaggerAccumulated >= StaggerThreshold)
	{
		// 스태거 발동
		StaggerAccumulated = 0.f;
		BB->SetValueAsBool(ABossAIController::BB_bIsStaggered, true);
	}
}

void AEnemyBoss::Die_Implementation()
{
	Super::Die_Implementation();

	UE_LOG(LogTemp, Warning, TEXT("Boss is Dead. GAME CLEAR"));
}


// ── 페이즈 ─────────────────────────────────────────────
void AEnemyBoss::EnterPhase(int32 NewPhase)
{
	if (CurrentPhase == NewPhase) return;

	CurrentPhase = NewPhase;
	UE_LOG(LogTemp, Warning, TEXT("[BOSS] Enter : Phase %d"), CurrentPhase);

	switch (CurrentPhase)
	{
	case 2:
		FlyHeight = 500.f;
		//SetFlying(true);
		bEclipseVeilUsed = false;
		break;

	case 3:
		FlyHeight = 600.f;
		//SetFlying(true);
		GetCharacterMovement()->MaxFlySpeed = 900.f;
		break;

	default:
		break;
	}
}


// ── 비행 전환 ─────────────────────────────────────────────────
void AEnemyBoss::SetFlying(bool bFly)
{
	bIsFlying = bFly;
	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	if (!MovementComp) return;

	if (bFly)
	{
		MovementComp->SetMovementMode(MOVE_Flying);
		MovementComp->GravityScale = 0.f;
		MovementComp->MaxFlySpeed = (CurrentPhase >= 3) ? 900.f : 700.f;
		MovementComp->BrakingDecelerationFlying = 2000.f;

		FVector Loc = GetActorLocation();
		Loc.Z = FlyHeight;
		SetActorLocation(Loc);
	}
	else
	{
		MovementComp->SetMovementMode(MOVE_Walking);
		MovementComp->GravityScale = 1.f;
	}
}

// ── 공격 실행 진입점 ──────────────────────────────────────────
void AEnemyBoss::ExecuteAttack(EBossAttackType Attack)
{
	AttackLastUsedList.Add(Attack, GetWorld()->GetTimeSeconds());

	switch (Attack)
	{
	case EBossAttackType::BloodBolt:     Attack_BloodBolt();     break;
	case EBossAttackType::WraithDrop:    Attack_WraithDrop();    break;
	case EBossAttackType::LunarBeam:     Attack_LunarBeam();     break;
	//case EBossAttackType::DamningTether: Attack_DamningTether(); break;
	//case EBossAttackType::MiasmaStep:    Defense_MiasmaStep();   break;
	case EBossAttackType::EclipseVeil:   Defense_EclipseVeil();  break;
	default: break;
	}
}

void AEnemyBoss::SetInvincible(bool bInvincible)
{
	SetCanBeDamaged(!bInvincible);
}


// ── 개별 공격 구현 ────────────────────────────────────────────
void AEnemyBoss::Attack_BloodBolt()
{
	int32 BoltCount = (CurrentPhase == 1) ? 3 : (CurrentPhase == 2) ? 5 :  5;
}

void AEnemyBoss::Attack_WraithDrop()
{
	if (!MinionClass || !AI) return;
	if (ActiveWraithCount > 0) return;

	int32 SpawnCount = (CurrentPhase >= 3) ? 6 : 3;

	
	UWorld* World = GetWorld();
	if (!World) return;

	for (int32 i = 0; i < SpawnCount; i++)
	{
		// 보스 위치에서 약간 떨어진 랜덤 위치 계산
		FVector SpawnLocation = GetActorLocation() + FVector(FMath::RandRange(-500, 500), FMath::RandRange(-500, 500), -FlyHeight);
		FRotator SpawnRotation = GetActorRotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		// 실제로 월드에 스폰!
		AEnemyMinion* Wraith = World->SpawnActor<AEnemyMinion>(MinionClass, SpawnLocation, SpawnRotation, SpawnParams);

		if (Wraith)
		{
			++ActiveWraithCount;
		}
	}

	AI->GetBlackboardComponent()->SetValueAsInt(ABossAIController::BB_ActiveWraithCount, ActiveWraithCount);

	UE_LOG(LogTemp, Warning, TEXT("[BOSS] Spawn : %d Wraith"), SpawnCount);
}

void AEnemyBoss::Attack_LunarBeam()
{
}

void AEnemyBoss::Attack_DamningTether()
{
}

void AEnemyBoss::Defense_MiasmaStep()
{
	// 1. 현재 위치에 안개 Niagara 이펙트 재생

	APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!Player) return;

	/*
	FVector ReverseDir = (GetActorLocation() - Player->GetActorLocation()).GetSafeNormal();
	FVector NewLoc = GetActorLocation() + ReverseDir * 1200.f;
	NewLoc.Z = FlyHeight;
	SetActorLocation(NewLoc);

	// 3. 텔레포트 직후 BloodBolt 기습 발사
	GetWorldTimerManager().SetTimerForNextTick([this]()
		{
			Attack_BloodBolt();
		});
		*/

}

void AEnemyBoss::Defense_EclipseVeil()
{
	if (!AI) return;

	bEclipseVeilUsed = true;

	// 무적 ON
	SetInvincible(true);

	// BB 갱신
	AI->GetBlackboardComponent()->SetValueAsBool(ABossAIController::BB_bCanReceiveDamage, false);

	// 연출: Level Sequence 또는 AnimMontage로 처리
	// 일정 시간 후 무적 해제
	FTimerHandle VeilTimer;
	GetWorldTimerManager().SetTimer(VeilTimer, [this]()
		{
			SetInvincible(false);
			SetCanBeDamaged(true);
			AI->GetBlackboardComponent()->SetValueAsBool(ABossAIController::BB_bCanReceiveDamage, true);
			UE_LOG(LogTemp, Warning, TEXT("[BOSS Attack] EclipseVeil End"));
		}, 8.f, false);

	UE_LOG(LogTemp, Warning, TEXT("[BOSS Attack] EclipseVeil Start"));
}


// ── 기타 ──────────────────────────────────────────────────────
void AEnemyBoss::OnWraithDied()
{
	if (!AI) return;

	ActiveWraithCount = FMath::Max(0, ActiveWraithCount - 1);

	AI->GetBlackboardComponent()->SetValueAsInt(ABossAIController::BB_ActiveWraithCount, ActiveWraithCount);
}


// ── Stagger ──────────────────────────────────────────────────────
void AEnemyBoss::UpdateStaggerThresholdByPhase()
{
	StaggerThreshold = (CurrentPhase == 1) ? 100.f
					: (CurrentPhase == 2) ? 200.f
					: 300.f;
}

