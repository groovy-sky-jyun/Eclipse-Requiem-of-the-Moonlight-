// Fill out your copyright notice in the Description page of Project Settings.


#include "SlashBeam.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "CombatInterface.h"

// Sets default values
ASlashBeam::ASlashBeam()
{
	PrimaryActorTick.bCanEverTick = false;

	SlashBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SlashBox"));
	SlashBox->SetBoxExtent(SlashExtent);
	SlashBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SlashBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	SlashBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = SlashBox;

	SlashMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SlashMesh"));
	SlashMesh->SetupAttachment(RootComponent);
	SlashMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void ASlashBeam::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASlashBeam::Activate(float Damage, AActor* DamageInstigator)
{
	SlashBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	TArray<AActor*> OverlappingActors;
	SlashBox->GetOverlappingActors(OverlappingActors);

	for (AActor* HitActor : OverlappingActors)
	{
		if (!HitActor || HitActor == DamageInstigator) continue;
		if (!HitActor->Implements<UCombatInterface>()) continue;

		FGameplayTag TargetTag = ICombatInterface::Execute_GetTeamTag(HitActor);
		FGameplayTag BossTag = ICombatInterface::Execute_GetTeamTag(DamageInstigator);
		if (TargetTag == BossTag) continue;

		ICombatInterface::Execute_HandleTakeDamage(HitActor, Damage, DamageInstigator);

		UE_LOG(LogTemp, Warning, TEXT("[SlashBeam] Hit"));
	}
	
	SlashBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(DestroyDelay);
}


