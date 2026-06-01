// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyMinion.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemyMinion::AEnemyMinion()
{
	MaxHealth = 200.f;
	CurrentHealth = MaxHealth;

	// 미니언은 보통 플레이어보다 이동 속도가 느림
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->MaxWalkSpeed = 300.f;
	}
}

void AEnemyMinion::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemyMinion::HandleTakeDamage_Implementation(float DamageAmount, AActor* Attacker)
{
	UE_LOG(LogTemp, Warning, TEXT("Minion Current HP: %f / %f"), CurrentHealth, MaxHealth);
	Super::HandleTakeDamage_Implementation(DamageAmount, Attacker);
	UE_LOG(LogTemp, Warning, TEXT("Minion Hit"));
}

void AEnemyMinion::Die_Implementation()
{
	Super::Die_Implementation();
	Destroy();
	UE_LOG(LogTemp, Warning, TEXT("Minion is Dead!!!"));
}

void AEnemyMinion::Attack()
{

}