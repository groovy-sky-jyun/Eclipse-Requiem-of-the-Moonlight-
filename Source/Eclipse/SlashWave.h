// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SlashWave.generated.h"

class UBoxComponent;

/**
 * 반달 참격 투사체. 발사된 방향으로 직진하다 첫 명중에서 소멸한다.
 */
UCLASS()
class ECLIPSE_API ASlashWave : public AActor
{
	GENERATED_BODY()

public:
	ASlashWave();

	virtual void Tick(float DeltaTime) override;

	/** 지정한 방향으로 발사한다. 사거리 끝에서 스스로 사라진다. */
	void Launch(const FVector& Direction, float InDamage, AActor* InDamageInstigator);

protected:
	/** 타격했으면 true. 참격은 그 자리에서 사라진다. */
	bool ApplyHit();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Settings|Components")
	TObjectPtr<UBoxComponent> SlashBox;

	UPROPERTY(VisibleAnywhere, Category = "Settings|Components")
	TObjectPtr<UStaticMeshComponent> SlashMesh;

	UPROPERTY(EditAnywhere, Category = "Settings|Slash")
	FVector SlashExtent = FVector(30.f, 150.f, 90.f);

	UPROPERTY(EditAnywhere, Category = "Settings|Slash")
	float SlashSpeed = 1800.f;

	UPROPERTY(EditAnywhere, Category = "Settings|Slash")
	float SlashRange = 1600.f;

private:
	UPROPERTY(Transient)
	TObjectPtr<AActor> DamageInstigator = nullptr;

	FVector MoveDirection = FVector::ZeroVector;

	float Damage = 0.f;

	float TraveledDistance = 0.f;

	bool bLaunched = false;
};
