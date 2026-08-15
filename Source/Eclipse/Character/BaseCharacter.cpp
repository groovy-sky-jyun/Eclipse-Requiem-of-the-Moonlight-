// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::HandleTakeDamage_Implementation(float DamageAmount, AActor* Attacker)
{
	if (IsDead_Implementation())
	{
		return;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.f, MaxHealth);

	FString AttackerName = Attacker ? Attacker->GetName() : TEXT("Unknown");
	UE_LOG(LogTemp, Warning, TEXT("[%s] Current HP: %f / %f"), *GetName(), CurrentHealth, MaxHealth);
}

void ABaseCharacter::Die_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("[%s] has died."), *GetName());
}
