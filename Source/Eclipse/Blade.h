// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Blade.generated.h"

class UBoxComponent;

UCLASS()
class ECLIPSE_API ABlade : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABlade();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor,
		class UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

protected:
	/** 검의 데미지 수치 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Damage")
	float BladeDamage = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Tags")
	FGameplayTag TeamTag;

	/** 검의 외형 (블루프린트에서 설정) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BladeMesh;

	/** 충돌 판정을 담당할 박스 콜리전 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr <UBoxComponent> CollisionBox;
};
