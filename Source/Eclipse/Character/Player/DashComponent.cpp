// Fill out your copyright notice in the Description page of Project Settings.


#include "DashComponent.h"
#include "Eclipse.h"
#include "BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "StaminaComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"

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

	OwnerCharacter->LandedDelegate.AddDynamic(this, &UDashComponent::HandleLanded);

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

	UAnimInstance* AnimInstance = GetAnimInstance();
	UAnimMontage* Montage = GetCurrentMontage();

	if (!AnimInstance || !Montage)
	{
		UE_LOG(LogEclipse, Warning, TEXT("[DashComponent] AnimInstance or Montage missing"));
		return;
	}

	const bool bIsFalling = OwnerMovement->IsFalling();
	if (bIsFalling && AirDashCount >= MaxAirDashCount) return;

	if (!StaminaComponent->TryConsume(DashCost)) return;

	if (AnimInstance->Montage_Play(Montage) <= 0.f)
	{
		UE_LOG(LogEclipse, Warning, TEXT("[DashComponent] Dash montage play failed"));
		return;
	}

	// 매개변수가 const가 아닌 참조라서 변수로 만들어 넘긴다.
	FOnMontageBlendingOutStarted BlendingOutDelegate = FOnMontageBlendingOutStarted::CreateUObject(this, &UDashComponent::HandleDashMontageBlendingOut);
	AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, Montage);

	bIsDashing = true;
	bIsAirDash = bIsFalling;
	bIsBackstep = !HasMoveInput();
	OwnerCharacter->SetInvincible(true);

	if (bIsAirDash)
	{
		++AirDashCount;
		StartAirDashMovement();
	}
	else
	{
		StartDashMovement();
	}
}

void UDashComponent::EndDash()
{
	if (!IsDashing()) return;

	OwnerCharacter->SetInvincible(false);

	if (bIsAirDash)
	{
		EndAirDashMovement();
	}
	else
	{
		EndDashMovement();
	}

	bIsDashing = false;
	bIsAirDash = false;
}

void UDashComponent::StartDashMovement()
{
	// 대시 동안 속도가 깎이지 않도록 마찰과 감속을 끈다.
	SavedGroundFriction = OwnerMovement->GroundFriction;
	SavedBrakingDecelerationWalking = OwnerMovement->BrakingDecelerationWalking;
	OwnerMovement->GroundFriction = 0.f;
	OwnerMovement->BrakingDecelerationWalking = 0.f;

	FVector DashDirection;
	float Speed;

	if (bIsBackstep)
	{
		//정면 유지한채 뒤로 이동
		DashDirection = -OwnerCharacter->GetActorForwardVector().GetSafeNormal2D();
		Speed = BackstepSpeed;
	}
	else
	{
		// 입력 방향으로 즉시 회전한다.(대시하는 순간의 방향을 보장하기 위함)
		DashDirection = OwnerCharacter->GetLastMovementInputVector().GetSafeNormal2D();
		OwnerCharacter->SetActorRotation(DashDirection.Rotation());
		Speed = DashSpeed;
	}

	// 수평 속도만 변경. 낙하 속도는 건드리지 않는다.
	OwnerMovement->Velocity = FVector(DashDirection.X * Speed, DashDirection.Y * Speed, OwnerMovement->Velocity.Z);
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

void UDashComponent::StartAirDashMovement()
{
	// 수평으로 날아가도록 중력과 낙하 속도를 없앤다.
	SavedGravityScale = OwnerMovement->GravityScale;
	OwnerMovement->GravityScale = 0.f;
	OwnerMovement->Velocity.Z = 0.f;

	StartDashMovement();
}

void UDashComponent::EndAirDashMovement()
{
	OwnerMovement->GravityScale = SavedGravityScale;

	EndDashMovement();
}

bool UDashComponent::CanDash() const
{
	if (!OwnerCharacter || !OwnerMovement || !StaminaComponent || !DashMontage) return false;

	if (OwnerMovement->IsFalling() && AirDashCount >= MaxAirDashCount) return false;

	return !IsDashing() && StaminaComponent->HasEnough(DashCost);
}

void UDashComponent::HandleLanded(const FHitResult& Hit)
{
	AirDashCount = 0;
}

void UDashComponent::HandleDashMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	EndDash();
}

UAnimInstance* UDashComponent::GetAnimInstance() const
{
	return (OwnerCharacter && OwnerCharacter->GetMesh()) ? OwnerCharacter->GetMesh()->GetAnimInstance() : nullptr;
}

UAnimMontage* UDashComponent::GetCurrentMontage() const
{
	return HasMoveInput() ? DashMontage : BackstepMontage;
}

bool UDashComponent::HasMoveInput() const
{
	return !OwnerCharacter->GetLastMovementInputVector().IsNearlyZero();
}
