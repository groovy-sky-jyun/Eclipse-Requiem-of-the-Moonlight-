// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAttackComponent.h"
#include "EnemyBoss.h"
#include "BossAttackBase.h"
#include "BossAttack_BloodBolt.h"
#include "BossAttack_ShadowCrash.h"
#include "BossAttack_WraithDrop.h"
#include "BossAttack_DarkSweep.h"
#include "BossAttack_LunarBeam.h"
#include "BossAttack_EclipseVeil.h"

UBossAttackComponent::UBossAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBossAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	Boss = Cast<AEnemyBoss>(GetOwner());
	if (!Boss)
	{
		UE_LOG(LogTemp, Error, TEXT("[BossAttack] Owner가 AEnemyBoss가 아니다."));
	}
}


// ── 공격 실행 진입점 ──────────────────────────────────────────
void UBossAttackComponent::ExecuteAttack(EBossAttackType Attack)
{
	if (!IsValid(Boss)) return;

	AttackLastUsedList.Add(Attack, GetWorld()->GetTimeSeconds());

	// 임시 브리지. (풀 데이터 생성 시 삭제)
	UClass* AttackClass = nullptr;
	switch (Attack)
	{
	case EBossAttackType::BloodBolt:     AttackClass = UBossAttack_BloodBolt::StaticClass();     break;
	case EBossAttackType::ShadowCrash:   AttackClass = UBossAttack_ShadowCrash::StaticClass();   break;
	case EBossAttackType::WraithDrop:    AttackClass = UBossAttack_WraithDrop::StaticClass();    break;
	case EBossAttackType::DarkSweep:     AttackClass = UBossAttack_DarkSweep::StaticClass();     break;
	case EBossAttackType::LunarBeam:     AttackClass = UBossAttack_LunarBeam::StaticClass();     break;
	case EBossAttackType::EclipseVeil:   AttackClass = UBossAttack_EclipseVeil::StaticClass();   break;
	default: return;
	}

	// 이전 공격이 아직 살아 있으면 타이머가 겹치므로 먼저 끊는다.
	if (IsValid(CurrentAttack) && CurrentAttack->IsRunning())
	{
		CurrentAttack->Cancel();
	}

	CurrentAttack = NewObject<UBossAttackBase>(this, AttackClass);
	CurrentAttack->Begin(Boss);
}

void UBossAttackComponent::CancelCurrent()
{
	if (IsValid(CurrentAttack))
	{
		CurrentAttack->Cancel();
	}
}

void UBossAttackComponent::NotifyAttackFinished()
{
	if (OnAttackFinishedDelegate.IsBound())
	{
		OnAttackFinishedDelegate.Broadcast();
		UE_LOG(LogTemp, Warning, TEXT("Attack Finished Broadcast"));
	}
}
