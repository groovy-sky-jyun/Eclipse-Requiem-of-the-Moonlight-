// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Blade.generated.h"

class UBoxComponent;
class APlayerCharacter;

UENUM(BlueprintType)
enum class EBladeState : uint8
{
	Idle,
	Attacking,
	Returning
};

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

	UFUNCTION()
	void UpdateIdleState(float DeltaTime);

	UFUNCTION()
	void Launch(const FVector& TargetLoc);

	UFUNCTION()
	const EBladeState GetCurrentState() const {	return CurrentState; }

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetBladeDamage(float NewDamage) { BladeDamage = NewDamage; }


protected:
	UPROPERTY(VisibleAnywhere, Category = "State")
	EBladeState CurrentState = EBladeState::Idle;

	FVector TargetAttackLocation = FVector::ZeroVector;

	TSet<TWeakObjectPtr<AActor>> HitActorsThisSwing; //한번의 Swing에 한 Enemy가 여러면 태격 되는것 방지


protected:
	/** 검의 데미지 수치 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Combat|Damage")
	float BladeDamage = 25.f;

	/** Owner 캐싱 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Combat|Owner")
	class APlayerCharacter* OwnerCharacter;

	/** 검의 외형 (블루프린트에서 설정) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BladeMesh;

	/** 충돌 판정을 담당할 박스 콜리전 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr <UBoxComponent> CollisionBox;


protected:
	/** 플레이어로부터의 상대적인 거리 (예: 왼쪽 뒤 100, 위로 50) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Floating")
	FVector AttachOffset = FVector(-20.f, -50.f, 50.f);

	/** 위치 따라오는 속도 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Floating")
	float FollowLocSpeed = 20.f;

	/** 방향 따라가는 속도 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Floating")
	float FollowRotSpeed = 20.f;

	/** 위아래로 흔들리는 진폭 (둥둥 떠다니는 높이) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Floating")
	float FloatAmplitude = 1.f;

	/** 흔들리는 속도 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Floating")
	float FloatSpeed = 2.f;


private:
	/** 누적된 시간 값 (Sin 계산용) */
	float RunningTime = 0.f;

	void SetCollisionActive(bool bActive);
};
