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

	SetHealth(MaxHealth);
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ABaseCharacter::AreHostile(AActor* A, AActor* B)
{
	if (!A || !B) return true;

	if (A == B) return false;

	if (!A->Implements<UCombatInterface>() || !B->Implements<UCombatInterface>()) return true;
	const FGameplayTag ATag = ICombatInterface::Execute_GetTeamTag(A);
	const FGameplayTag BTag = ICombatInterface::Execute_GetTeamTag(B);

	if (!ATag.IsValid() || !BTag.IsValid()) return true;

	return ATag != BTag;
}

void ABaseCharacter::TakeCombatDamage_Implementation(float DamageAmount, AActor* Attacker)
{
	if (IsDead_Implementation()) return;
	if (!CanBeDamaged()) return;
	if (DamageAmount <= 0.f) return;
	if (!AreHostile(this, Attacker)) return;

	SetHealth(CurrentHealth - DamageAmount);

	const bool bLethal = (CurrentHealth <= 0.f);

	OnDamaged(DamageAmount, Attacker, bLethal);

	if (bLethal)
	{
		ICombatInterface::Execute_Die(this);
	}

}

void ABaseCharacter::Die_Implementation()
{
	if (bIsDead) return;
	bIsDead = true;

	if (CurrentHealth > 0.f) SetHealth(0.f);

	// 시체가 길을 막거나 남의 공격 판정을 대신 먹지 않도록 정리
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->StopMovementImmediately();
		Movement->DisableMovement();
	}

	OnDeath();

	UE_LOG(LogTemp, Warning, TEXT("[%s] has died."), *GetName());
}

void ABaseCharacter::SetHealth(float NewHealth)
{
	CurrentHealth = FMath::Clamp(NewHealth, 0.f, MaxHealth);

	UE_LOG(LogTemp, Warning, TEXT("[%s] HP: %f / %f"), *GetName(), CurrentHealth, MaxHealth);

	OnHealthChangedDelegate.Broadcast(CurrentHealth, MaxHealth);
}
