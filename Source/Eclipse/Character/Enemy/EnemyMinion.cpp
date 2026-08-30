// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyMinion.h"
#include "EnemyBoss.h"
#include "WraithAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"

AEnemyMinion::AEnemyMinion()
{
	MaxHealth = 150.f;
}

void AEnemyMinion::BeginPlay()
{
	Super::BeginPlay();

	AI = Cast<AWraithAIController>(GetController());
	if (!AI)
	{
		UE_LOG(LogTemp, Error, TEXT("[Minion] AI Controller is NULL"));
		return;
	}

	BB = AI->GetBlackboardComponent();
	if (!BB)
	{
		UE_LOG(LogTemp, Error, TEXT("[Minion] Blackboard is NULL"));
		return;
	}

	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->MaxWalkSpeed = FMath::RandRange(80, 400);

	}
}

void AEnemyMinion::OnDeath()
{
	if (IsValid(OwnerBoss))
	{
		OwnerBoss->OnWraithDied();
	}
	
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 오물 남기기 로직 작성
	
	SetLifeSpan(0.5f);
}

void AEnemyMinion::AttackStart()
{
	FVector StartLoc = GetActorLocation();
	FVector EndLoc = StartLoc + (GetActorForwardVector() * 100.f);

	float AttackRadius = 50.f;

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		StartLoc,
		EndLoc,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(AttackRadius),
		Params
		);

	DrawDebugCapsule(
		GetWorld(),
		(StartLoc + EndLoc) * 0.5f,
		50.f,
		AttackRadius,
		FQuat::Identity,
		bHit ? FColor::Red : FColor::Green,
		false,
		2.f
	);

	if (bHit && HitResult.GetActor())
	{
		AActor* HitActor = HitResult.GetActor();

		if (HitActor->Implements<UCombatInterface>())
		{
			ICombatInterface::Execute_TakeCombatDamage(HitActor, 10.f, this);
		}
	}

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AEnemyMinion::AttackEnd, 0.5f, false);
}

// Anim Notify에서 호출
void AEnemyMinion::AttackEnd()
{
	if (OnAttackFinishedDelegate.IsBound())
	{
		OnAttackFinishedDelegate.Broadcast();
	}
}
