// Fill out your copyright notice in the Description page of Project Settings.


#include "DashComponent.h"
#include "Eclipse.h"
#include "BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "StaminaComponent.h"

UDashComponent::UDashComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDashComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = GetOwner<ABaseCharacter>();
	if (!OwnerCharacter)
	{
		UE_LOG(LogEclipse, Warning, TEXT("[DashComponent] Owner is not BaseCharacter"));
		return;
	}

	OwnerMovement = OwnerCharacter->GetCharacterMovement();
	if (!OwnerMovement)
	{
		UE_LOG(LogEclipse, Warning, TEXT("[DashComponent] OwnerMovement is null"));
		return;
	}

	StaminaComponent = GetOwner()->FindComponentByClass<UStaminaComponent>();
	if (!StaminaComponent)
	{
		UE_LOG(LogEclipse, Warning, TEXT("[DashComponent] StaminaComponent is null"));
		return;
	}
}

void UDashComponent::StartDash()
{
	if (!OwnerCharacter || !OwnerMovement || !StaminaComponent) return;
	if (IsDashing()) return;
	if (!StaminaComponent->TryConsume(DashCost)) return;

	bIsDashing = true;
	OwnerCharacter->SetInvincible(true);
	StartDashMovement();

	// 후에 Montage_SetEndDelegate 로 변경
	OwnerCharacter->GetWorldTimerManager().SetTimer(DashHandle, this, &UDashComponent::EndDash, DashDuration, false);
}

void UDashComponent::EndDash()
{
	if (!IsDashing()) return;

	OwnerCharacter->SetInvincible(false);
	EndDashMovement();
	bIsDashing = false;
}

void UDashComponent::StartDashMovement()
{
	// 대시 동안 속도가 깎이지 않도록 마찰과 감속을 끈다.
	SavedGroundFriction = OwnerMovement->GroundFriction;
	SavedBrakingDecelerationWalking = OwnerMovement->BrakingDecelerationWalking;
	OwnerMovement->GroundFriction = 0.f;
	OwnerMovement->BrakingDecelerationWalking = 0.f;

	// 수평 속도만 변경. 낙하 속도는 건드리지 않는다.
	const FVector DashDirection = OwnerCharacter->GetActorForwardVector().GetSafeNormal2D();
	OwnerMovement->Velocity = FVector(DashDirection.X * DashSpeed, DashDirection.Y * DashSpeed, OwnerMovement->Velocity.Z);	
}

void UDashComponent::EndDashMovement()
{
	OwnerMovement->GroundFriction = SavedGroundFriction;
	OwnerMovement->BrakingDecelerationWalking = SavedBrakingDecelerationWalking;

	// 즉시 멈추면 이질감이 든다. 걷기 속도까지만 낮추고 나머지 감속은 CMC에 맡긴다.
	const FVector HorizontalVelocity(OwnerMovement->Velocity.X, OwnerMovement->Velocity.Y, 0.f);
	const FVector ExitVelocity = HorizontalVelocity.GetSafeNormal() * FMath::Min(HorizontalVelocity.Size(), OwnerMovement->MaxWalkSpeed);

	OwnerMovement->Velocity = FVector(ExitVelocity.X, ExitVelocity.Y, OwnerMovement->Velocity.Z);
}

bool UDashComponent::CanDash() const
{
	if (!OwnerCharacter || !OwnerMovement || !StaminaComponent) return false;

	return !IsDashing() && StaminaComponent->HasEnough(DashCost);
}
