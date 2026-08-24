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

	/**
	 * 이미 한 번 명중했는지.
	 * Destroy()는 예약일 뿐이라 같은 프레임 안의 추가 히트를 막아주지 않는다.
	 * "구체 하나당 한 번"은 우연이 아니라 여기서 보장한다.
	 */
	bool bHasHit = false;


public:
	void SetDamage(float InDamage) { Damage = InDamage; }

};
