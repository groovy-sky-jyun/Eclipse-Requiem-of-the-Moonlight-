// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "SlashBeam.generated.h"


/**
* Eclipse Veil 참격 액터
* 긴 직사각형 판정
*/
UCLASS()
class ECLIPSE_API ASlashBeam : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASlashBeam();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	void Activate(float Damage, AActor* DamageInstigator);

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UBoxComponent> SlashBox;

	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<UStaticMeshComponent> SlashMesh;

	UPROPERTY(EditAnywhere, Category="Slash")
	FVector SlashExtent = FVector(4000.f, 80.f, 200.f);

	UPROPERTY(EditAnywhere, Category="Slash")
	float DestroyDelay = 0.4f;

};
