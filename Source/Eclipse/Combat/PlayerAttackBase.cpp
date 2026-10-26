// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAttackBase.h"
#include "Eclipse.h"
#include "PlayerAttackComponent.h"
#include "BaseCharacter.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "DrawDebugHelpers.h"

// ── 실행 제어 ─────────────────────────────────────────────
void UPlayerAttackBase::EnterStartup(UPlayerAttackComponent* InComponent, const FGameplayTag& StepTag)
{
	if (!InComponent) return;

	Component = InComponent;

	UAnimInstance* AnimInstance = GetAnimInstance();

	const FPlayerAttackStep StepData = GetStepDataByTag(StepTag);

	if (!AnimInstance || !StepData.Montage)
	{
		UE_LOG(LogPlayerAttack, Warning, TEXT("PlayerAttackBase::EnterStartup : AnimInstance or Montage missing (%s)"), *StepTag.ToString());
		return;
	}

	// 실행 중이면 새 몽타주가 이전 몽타주를 끊는다.
	if (AnimInstance->Montage_Play(StepData.Montage) <= 0.f)
	{
		UE_LOG(LogPlayerAttack, Warning, TEXT("PlayerAttackBase::EnterStartup : Montage play failed (%s)"), *StepTag.ToString());
		return;
	}

	CurrentState = EPlayerAttackState::Startup;
	CurrentMontage = StepData.Montage;
	CurrentStep = StepData;

	// 매개변수가 const가 아닌 참조라서 변수로 만들어 넘긴다.
	FOnMontageEnded EndDelegate = FOnMontageEnded::CreateUObject(this, &UPlayerAttackBase::HandleMontageEnded);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, StepData.Montage);

	OnStartup();
}

void UPlayerAttackBase::EnterActive()
{
	HitActors.Reset();
	PrevTracePoints = GetTracePoints();
	CurrentState = EPlayerAttackState::Active;

	OnActive();
}

void UPlayerAttackBase::EnterRecovery()
{
	CurrentState = EPlayerAttackState::Recovery;

	OnRecovery();
}


void UPlayerAttackBase::HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// 끊긴 경우(새 공격 재생, Cancel)는 끊은 쪽에서 이미 정리했다.
	if (bInterrupted) return;


	Finish();
}

void UPlayerAttackBase::Cancel()
{
	OnCancel();

	StopMontage();

	Finish();
}

void UPlayerAttackBase::Finish()
{
	if (!Component) return;

	CurrentState = EPlayerAttackState::Idle;
	CurrentMontage = nullptr;

	OnFinish();

	Component->EndAttack();
}

void UPlayerAttackBase::TraceWeapon()
{
	const TArray<FVector> CurrentPoints = GetTracePoints();
	if (CurrentPoints.Num() != PrevTracePoints.Num()) return;

	ABaseCharacter* Owner = GetOwnerCharacter();
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);

	const bool bDrawTraceDebug = Component->ShouldDrawTraceDebug();

	for (int32 i = 0; i < CurrentPoints.Num(); ++i)
	{
		TArray<FHitResult> Hits;
		GetWorld()->SweepMultiByObjectType(
			Hits,
			PrevTracePoints[i],
			CurrentPoints[i],
			FQuat::Identity,
			FCollisionObjectQueryParams(ECC_Pawn),
			FCollisionShape::MakeSphere(CurrentStep.HitRadius),
			Params
		);

		for (const FHitResult& Hit : Hits)
		{
			ApplyHit(Hit.GetActor());
		}

		if (bDrawTraceDebug)
		{
			const FColor Color = Hits.Num() > 0 ? FColor::Red : FColor::Green;
			DrawDebugLine(GetWorld(), PrevTracePoints[i], CurrentPoints[i], Color, false, 1.f);
			DrawDebugSphere(GetWorld(), CurrentPoints[i], CurrentStep.HitRadius, 8, Color, false, 1.f);
		}
	}

	// 현재 검날 위치
	if (bDrawTraceDebug && CurrentPoints.Num() > 0)
	{
		DrawDebugLine(GetWorld(), CurrentPoints[0], CurrentPoints.Last(), FColor::Yellow, false, 1.f, 0, 1.f);
	}

	PrevTracePoints = CurrentPoints;
}


// ── 헬퍼 ─────────────────────────────────────────────
UWorld* UPlayerAttackBase::GetWorld() const
{
	return Component ? Component->GetWorld() : nullptr;
}

ABaseCharacter* UPlayerAttackBase::GetOwnerCharacter() const
{
	return Component ? Cast<ABaseCharacter>(Component->GetOwner()) : nullptr;
}

UAnimInstance* UPlayerAttackBase::GetAnimInstance() const
{
	ACharacter* Character = Component ? Cast<ACharacter>(Component->GetOwner()) : nullptr;
	return (Character && Character->GetMesh()) ? Character->GetMesh()->GetAnimInstance() : nullptr;
}

void UPlayerAttackBase::StopMontage()
{
	UAnimInstance* AnimInstance = GetAnimInstance();
	if (!AnimInstance || !CurrentMontage) return;

	AnimInstance->Montage_Stop(CurrentMontage->GetDefaultBlendOutTime(), CurrentMontage);
}

TArray<FVector> UPlayerAttackBase::GetTracePoints()
{
	TArray<FVector> Points;

	ACharacter* Character = Component ? Cast<ACharacter>(Component->GetOwner()) : nullptr;
	USkeletalMeshComponent* Mesh = Character ? Character->GetMesh() : nullptr;
	if (!Mesh) return Points;

	const FVector Base = Mesh->GetSocketLocation(Component->GetWeaponBaseSocket());
	const FVector Tip = Mesh->GetSocketLocation(Component->GetWeaponTipSocket());

	for (int32 i = 0; i < TraceSampleCount; i++)
	{
		Points.Add(FMath::Lerp(Base, Tip, (float)i / (TraceSampleCount - 1)));
	}

	return Points;
}

void UPlayerAttackBase::ApplyHit(AActor* Target)
{
	if (!Target || HitActors.Contains(Target)) return;
	if (!Target->Implements<UCombatInterface>()) return;

	HitActors.Add(Target);

	UE_LOG(LogPlayerAttack, Log, TEXT("PlayerAttackBase::ApplyHit : %s, Damage %.1f"), *Target->GetName(), CurrentStep.CombatDamage.Damage);

	ICombatInterface::Execute_TakeCombatDamage(Target, CurrentStep.CombatDamage, GetOwnerCharacter());
}

TArray<FGameplayTag> UPlayerAttackBase::GetAttackTags()
{
	TArray<FGameplayTag> Tags;
	for (const auto& Data : AttackStepData)
	{
		if (!Data.StepTag.IsValid())
		{
			UE_LOG(LogPlayerAttack, Warning, TEXT("PlayerAttack::GetAttackTags : Empty StepTag"));
			continue;
		}
		if (Tags.Contains(Data.StepTag))
		{
			UE_LOG(LogPlayerAttack, Warning, TEXT("PlayerAttack::GetAttackTags : Duplicate StepTag %s"),*Data.StepTag.ToString());
			continue;
		}

		Tags.Add(Data.StepTag);
	}
	return Tags;
}





FPlayerAttackStep UPlayerAttackBase::GetStepDataByTag(const FGameplayTag& StepTag)
{
	for (const auto& Data : AttackStepData)
	{
		if (StepTag == Data.StepTag)
		{
			return Data;
		}
	}

	return FPlayerAttackStep();
}



