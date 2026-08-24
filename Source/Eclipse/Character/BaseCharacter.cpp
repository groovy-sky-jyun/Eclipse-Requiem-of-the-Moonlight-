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
	CurrentHealth = MaxHealth;
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ABaseCharacter::AreHostile(AActor* A, AActor* B)
{
	// 공격자가 없으면 환경 데미지로 본다. 낙하나 장판에는 팀이 없다.
	if (!A || !B) return true;

	if (A == B) return false;

	if (!A->Implements<UCombatInterface>() || !B->Implements<UCombatInterface>()) return true;

	const FGameplayTag TagA = ICombatInterface::Execute_GetTeamTag(A);
	const FGameplayTag TagB = ICombatInterface::Execute_GetTeamTag(B);

	if (!TagA.IsValid() || !TagB.IsValid()) return true;

	// 지금 팀은 Team.Player / Team.Enemy 둘뿐이라 정확 비교로 충분하다.
	// Team.Enemy.Elite 같은 하위 태그가 생기면 MatchesTag로 바꾼다.
	return TagA != TagB;
}

void ABaseCharacter::HandleTakeDamage_Implementation(float DamageAmount, AActor* Attacker)
{
	if (IsDead_Implementation()) return;

	if (!CanBeDamaged()) return;

	// 아군 오사 차단. 공격 코드 6곳에 흩어져 있던 판정을 여기 하나로 모았다.
	if (!AreHostile(this, Attacker)) return;

	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.f, MaxHealth);

	FString AttackerName = Attacker ? Attacker->GetName() : TEXT("Unknown");
	UE_LOG(LogTemp, Warning, TEXT("[%s] Current HP: %f / %f"), *GetName(), CurrentHealth, MaxHealth);

	const bool bLethal = (CurrentHealth <= 0.f);

	// Die()보다 먼저 불러야 피격 반응이 사망 연출을 덮어쓰지 않는다.
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

	UE_LOG(LogTemp, Warning, TEXT("[%s] has died."), *GetName());

	HandleDeath();
}

void ABaseCharacter::HandleDeath()
{
	// IsDead 가드가 추가 데미지는 막아주지만 콜리전은 그대로 남는다.
	// 시체가 길을 막거나 남의 공격 판정을 대신 먹지 않도록 여기서 정리한다.
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->StopMovementImmediately();
		Movement->DisableMovement();
	}
}
