// Fill out your copyright notice in the Description page of Project Settings.


#include "StaminaComponent.h"
#include "Eclipse.h"
#include "TimerManager.h"
#include "Engine/World.h"

UStaminaComponent::UStaminaComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	// BeginPlay 전에 UI가 읽어도 가득 찬 값이 나오게 한다.
	CurrentStamina = MaxStamina;
}

void UStaminaComponent::BeginPlay()
{
	Super::BeginPlay();

	SetCurrentStamina(MaxStamina);
}

bool UStaminaComponent::HasEnough(float Cost) const
{
	return CurrentStamina >= Cost;
}

bool UStaminaComponent::TryConsume(float Cost)
{
	if (!HasEnough(Cost)) return false;

	// 소모하는 동안은 회복하지 않는다.
	StopRegen();

	SetCurrentStamina(FMath::Max(0.f, CurrentStamina - Cost));

	UWorld* World = GetWorld();
	if (!World || RegenDelay <= 0.f)
	{
		StartRegen();
		return true;
	}

	World->GetTimerManager().SetTimer(
		RegenDelayHandle,
		FTimerDelegate::CreateUObject(this, &UStaminaComponent::StartRegen),
		RegenDelay,
		false);

	return true;
}

void UStaminaComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SetCurrentStamina(FMath::Min(MaxStamina, CurrentStamina + RegenPerSecond * DeltaTime));

	if (CurrentStamina >= MaxStamina)
	{
		StopRegen();
	}
}

void UStaminaComponent::StartRegen()
{
	if (RegenPerSecond <= 0.f || CurrentStamina >= MaxStamina) return;

	SetComponentTickEnabled(true);
}

void UStaminaComponent::StopRegen()
{
	SetComponentTickEnabled(false);
}

void UStaminaComponent::SetCurrentStamina(float NewStamina)
{
	CurrentStamina = NewStamina;
	OnStaminaChangedDelegate.Broadcast(CurrentStamina, MaxStamina);
}
