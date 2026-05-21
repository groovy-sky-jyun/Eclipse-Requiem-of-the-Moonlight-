// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Attack_BloodBolt.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

/**
 * 보스 BloodBolt 투사체
 * - 발사 시점의 플레이어 위치를 향해 직선으로 날아감
 * - 플레이어에 명중 시 데미지 + CC(경직) 적용
 * - 일정 거리 또는 시간 초과 시 자동 소멸
 */
UCLASS()
class ECLIPSE_API AAttack_BloodBolt : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAttack_BloodBolt();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	void Launch(const FVector& Direction);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:
	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<USphereComponent> CollisionSphere;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BoltMesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float Damage = 30.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float Speed = 1800.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float LifeSpanDuration = 4.f;


public:
	void SetDamage(float InDamage) { Damage = InDamage; }
	void SetSpeed(float InSpeed);

};
