// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "Eclipse.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "EclipseGameMode.h"
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

void ABaseCharacter::TakeCombatDamage_Implementation(const FCombatDamage& DamageInfo, AActor* Attacker)
{
	if (IsDead_Implementation()) return;
	if (!CanBeDamaged()) return;
	if (DamageInfo.Damage <= 0.f) return;
	if (!AreHostile(this, Attacker)) return;

	SetHealth(CurrentHealth - DamageInfo.Damage);

	const bool bLethal = (CurrentHealth <= 0.f);

	OnDamaged(DamageInfo, Attacker, bLethal);

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

	if (AEclipseGameMode* GameMode = AEclipseGameMode::Get(this))
	{
		GameMode->NotifyCharacterDied(this);
	}

	UE_LOG(LogEclipse, Log, TEXT("[%s] has died."), *GetName());

	// GameMode 구독이 끝난 뒤 재생해야 몽타주가 없을 때의 즉시 방송도 전달된다.
	PlayDeathMontage();
}

void ABaseCharacter::PlayDeathMontage()
{
	UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;

	if (!DeathMontage || !AnimInstance || AnimInstance->Montage_Play(DeathMontage) <= 0.f)
	{
		UE_LOG(LogEclipse, Warning, TEXT("[%s] Death montage not played"), *GetName());
		OnDeathMotionFinishedDelegate.Broadcast(this);
		return;
	}

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &ABaseCharacter::HandleDeathMontageEnded);

	// 나중에 타이머나 AnimNotify로 변경 필요. (지금은 AM의 EnableAutoBlendOut = true)
	AnimInstance->Montage_SetEndDelegate(EndDelegate, DeathMontage);
}

void ABaseCharacter::HandleDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// 몽타주가 도중에 끊겨도 결과 화면은 떠야 하므로 bInterrupted는 구분하지 않는다.
	OnDeathMotionFinishedDelegate.Broadcast(this);
}

void ABaseCharacter::SetHealth(float NewHealth)
{
	CurrentHealth = FMath::Clamp(NewHealth, 0.f, MaxHealth);

	UE_LOG(LogEclipse, Verbose, TEXT("[%s] HP: %f / %f"), *GetName(), CurrentHealth, MaxHealth);

	OnHealthChangedDelegate.Broadcast(CurrentHealth, MaxHealth);
}
