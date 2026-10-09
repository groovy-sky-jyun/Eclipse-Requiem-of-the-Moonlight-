// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DashComponent.generated.h"

class ABaseCharacter;
class UCharacterMovementComponent;
class UStaminaComponent;
/**
 * 플레이어 대시. 실행, 이동, 무적을 담당한다.
 */
UCLASS(ClassGroup = (Player), meta = (BlueprintSpawnableComponent))
class ECLIPSE_API UDashComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDashComponent();

protected:
	virtual void BeginPlay() override;


public:
	void StartDash();

	bool CanDash() const;

	bool IsDashing() const { return bIsDashing; }


protected:
	void EndDash();

	void StartDashMovement();

	void EndDashMovement();


protected:
	UPROPERTY()
	TObjectPtr<ABaseCharacter> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> OwnerMovement;

	UPROPERTY()
	TObjectPtr<UStaminaComponent> StaminaComponent;


protected:
	// 대시 중 유지하는 속도. 이동 거리는 DashSpeed x DashDuration이다.
	UPROPERTY(EditAnywhere, Category = "Settings|Dash", meta = (ClampMin = "0.0"))
	float DashSpeed = 4000.f;

	UPROPERTY(EditAnywhere, Category = "Settings|Dash", meta = (ClampMin = "0.0"))
	float DashCost = 20.f;

	// 대시 중으로 보는 시간(초). 이 동안 재입력을 막는다.
	UPROPERTY(EditAnywhere, Category = "Settings|Dash", meta = (ClampMin = "0.05"))
	float DashDuration = 0.1f;

	FTimerHandle DashHandle;

	// 대시 동안 0으로 바꾸므로 원래 값을 보관한다.
	float SavedGroundFriction = 0.f;

	float SavedBrakingDecelerationWalking = 0.f;


private:
	bool bIsDashing = false;
};
