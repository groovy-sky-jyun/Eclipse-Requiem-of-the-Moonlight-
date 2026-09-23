// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StaminaComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStaminaChanged, float /*Current*/, float /*Max*/);

/**
 * 대시 같은 행동이 소모하는 자원. 소비자가 누구인지는 알지 않는다.
 */
UCLASS(ClassGroup = (Player), meta = (BlueprintSpawnableComponent))
class ECLIPSE_API UStaminaComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStaminaComponent();

	/** 여유가 있으면 소모하고 true. 부족하면 아무것도 하지 않는다. */
	bool TryConsume(float Cost);

	/** 소모하지 않고 여유만 확인한다. */
	bool HasEnough(float Cost) const;

	float GetCurrentStamina() const { return CurrentStamina; }
	float GetMaxStamina() const { return MaxStamina; }

	// 값이 바뀔 때마다 방송한다. (소모, 회복)
	FOnStaminaChanged OnStaminaChangedDelegate;


protected:
	virtual void BeginPlay() override;

	/** 회복 중에만 켜진다. */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, Category = "Settings|Stamina", meta = (ClampMin = "1.0"))
	float MaxStamina = 100.f;

	UPROPERTY(EditAnywhere, Category = "Settings|Stamina", meta = (ClampMin = "0.0"))
	float RegenPerSecond = 20.f;

	// 마지막 소모 후 이 시간(초)이 지나야 회복이 시작된다.
	UPROPERTY(EditAnywhere, Category = "Settings|Stamina", meta = (ClampMin = "0.0"))
	float RegenDelay = 0.8f;


private:
	// 지연이 끝나면 호출. 회복이 필요한지 확인하고 켠다.
	void StartRegen();
	void StopRegen();

	// 값을 바꾸고 OnStaminaChangedDelegate를 방송한다.
	void SetCurrentStamina(float NewStamina);

	float CurrentStamina = 0.f;

	FTimerHandle RegenDelayHandle;
};
