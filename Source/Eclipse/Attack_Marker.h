// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Attack_Marker.generated.h"

class UCylinderComponent;
class UDecalComponent;

/**
* [Boss Attack 위치 경고 표시]
* 빨간 원형으로 공격 위치와 범위 미리 알림
* - Shadow Crash
* - 
*/
UCLASS()
class ECLIPSE_API AAttack_Marker : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAttack_Marker();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere, Category = "Setting|Marker")
	float AttackRangeRadius = 250.f;

	UPROPERTY(EditAnywhere, Category = "Setting|Marker")
	float LifeSpanDuration = 1.8f;

	UFUNCTION()
	void SetAttackRange(float Radius) { AttackRangeRadius = Radius; }


protected:
	UPROPERTY(VisibleAnywhere, Category="Setting|Component")
	TObjectPtr<UStaticMeshComponent> AttackMarkerMesh;

};
