// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/HitResult.h"
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

	void StartAirDashMovement();

	void EndAirDashMovement();

	// 착지하면 공중 대시 횟수를 초기화한다.
	UFUNCTION()
	void HandleLanded(const FHitResult& Hit);


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

	float SavedGravityScale = 1.f;

	UPROPERTY(EditAnywhere, Category = "Settings|AirDash")
	int32 MaxAirDashCount = 1;


private:
	bool bIsDashing = false;

	// 시작할 때 기록한다. 대시 도중 착지해도 종료는 시작한 방식으로 처리한다.
	bool bIsAirDash = false;

	int32 AirDashCount = 0;
};
