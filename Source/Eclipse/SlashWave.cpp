// Fill out your copyright notice in the Description page of Project Settings.


#include "SlashWave.h"
#include "Eclipse.h"
#include "CombatInterface.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"

ASlashWave::ASlashWave()
{
	PrimaryActorTick.bCanEverTick = true;

	SlashBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SlashBox"));
	SlashBox->SetBoxExtent(SlashExtent);
	SlashBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SlashBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	SlashBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = SlashBox;

	SlashMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SlashMesh"));
	SlashMesh->SetupAttachment(RootComponent);
	SlashMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASlashWave::Launch(const FVector& Direction, float InDamage, AActor* InDamageInstigator)
{
	MoveDirection = Direction.GetSafeNormal();
	if (MoveDirection.IsNearlyZero())
	{
		UE_LOG(LogEclipse, Error, TEXT("[SlashWave] Launch direction is zero"));
		Destroy();
		return;
	}

	Damage = InDamage;
	DamageInstigator = InDamageInstigator;
	TraveledDistance = 0.f;
	bLaunched = true;

	SetActorRotation(MoveDirection.Rotation());

	// 사거리 도달 전에 Tick이 멈춰도 남지 않도록 하는 안전망
	SetLifeSpan(SlashRange / SlashSpeed + 1.f);
}

void ASlashWave::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bLaunched) return;

	const float Step = SlashSpeed * DeltaTime;

	AddActorWorldOffset(MoveDirection * Step);
	TraveledDistance += Step;

#if ENABLE_DRAW_DEBUG
	DrawDebugBox(GetWorld(), GetActorLocation(), SlashExtent, GetActorQuat(), FColor::Red, false, 0.05f);
#endif

	if (ApplyHit() || TraveledDistance >= SlashRange)
	{
		Destroy();
	}
}

bool ASlashWave::ApplyHit()
{
	TArray<AActor*> OverlappingActors;
	SlashBox->GetOverlappingActors(OverlappingActors);

	for (AActor* HitActor : OverlappingActors)
	{
		if (!HitActor || HitActor == DamageInstigator) continue;
		if (!HitActor->Implements<UCombatInterface>()) continue;

		// 팀 판정은 맞는 쪽(ABaseCharacter::TakeCombatDamage)이 한다.
		ICombatInterface::Execute_TakeCombatDamage(HitActor, Damage, DamageInstigator);

		UE_LOG(LogEclipse, Log, TEXT("[SlashWave] Hit : %s"), *HitActor->GetName());

		return true;
	}

	return false;
}
