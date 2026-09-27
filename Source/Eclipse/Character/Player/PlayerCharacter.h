// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "GameplayTagContainer.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;
class ABlade;
class UStaminaComponent;


UCLASS()
class ECLIPSE_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()


public:
	/** Constructor */
	APlayerCharacter();


protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void JumpStart(const FInputActionValue& Value);
	void JumpEnd(const FInputActionValue& Value);
	void Dash(const FInputActionValue& Value);
	void BasicAttack(const FInputActionValue& Value);
	void FirstSpecialAttack(const FInputActionValue& Value);
	void SecondSpecialAttack(const FInputActionValue& Value);
	void UltimateAttack(const FInputActionValue& Value);


public:
	UFUNCTION(Category = "Input")
	virtual void DoMove(float Right, float Forward);

	UFUNCTION(Category = "Input")
	virtual void DoLook(float Yaw, float Pitch);

	UFUNCTION(Category = "Input")
	void DoDash();

	UFUNCTION(Category = "Input")
	void DoBasicAttack();

	UFUNCTION(Category = "Input")
	void DoFirstSpecialAttack();

	UFUNCTION(Category = "Input")
	void DoSecondSpecialAttack();

	UFUNCTION(Category = "Input")
	void DoUltimateAttack();



protected:
	// 피드백(히트 VFX, 사운드, 데미지 넘버)은 항상 재생한다.
	// bLethal이면 리액션(경직, 넉백, 피격 모션)은 생략한다.
	virtual void OnDamaged(const FCombatDamage& DamageInfo, AActor* Attacker, bool bLethal) override {};

	virtual void OnDeath() override {};


protected:
	// Input Mapping Context는 AEclipsePlayerController가 소유한다. 여기서는 액션만 다룬다.

	UPROPERTY(EditAnywhere, Category = "Settings|Input")
	TObjectPtr<UInputAction> IA_Move;

	UPROPERTY(EditAnywhere, Category = "Settings|Input")
	TObjectPtr<UInputAction> IA_Look;

	UPROPERTY(EditAnywhere, Category = "Settings|Input")
	TObjectPtr<UInputAction> IA_Jump;

	UPROPERTY(EditAnywhere, Category = "Settings|Input")
	TObjectPtr<UInputAction> IA_Dash;

	UPROPERTY(EditAnywhere, Category = "Settings|Input")
	TObjectPtr<UInputAction> IA_Attack;

	UPROPERTY(EditAnywhere, Category = "Settings|Input")
	TObjectPtr<UInputAction> IA_FirstSpecialAttack;

	UPROPERTY(EditAnywhere, Category = "Settings|Input")
	TObjectPtr<UInputAction> IA_SecondSpecialAttack;

	UPROPERTY(EditAnywhere, Category = "Settings|Input")
	TObjectPtr<UInputAction> IA_UltimateAttack;

	// 대시 중 유지하는 속도. 이동 거리는 DashSpeed x DashDuration이다.
	UPROPERTY(EditAnywhere, Category = "Settings|Input|Dash", meta = (ClampMin = "0.0"))
	float DashSpeed = 4000.f;

	UPROPERTY(EditAnywhere, Category = "Settings|Input|Dash", meta = (ClampMin = "0.0"))
	float DashCost = 20.f;

	// 대시 중으로 보는 시간(초). 이 동안 재입력을 막는다.
	UPROPERTY(EditAnywhere, Category = "Settings|Input|Dash", meta = (ClampMin = "0.05"))
	float DashDuration = 0.1f;


public:
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(BlueprintPure, Category = "Stamina")
	UStaminaComponent* GetStaminaComponent() const { return StaminaComponent; }


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaminaComponent> StaminaComponent;


protected:
	/** 스태미나를 제외한 대시 조건. 소모는 이 검사를 통과한 뒤에 한다. */
	bool CanDash() const;

	// DashDuration이 지나면 호출. 대시 상태를 풀고 이동 설정을 되돌린다.
	void EndDash();

	/** 마찰을 끄고 대시 속도를 넣는다. StopDashMovement와 짝이다. */
	void StartDashMovement();

	/** 마찰을 되돌리고 속도를 걷기 속도 이하로 낮춘다. */
	void StopDashMovement();

	bool bIsDashing = false;

	FTimerHandle DashHandle;

	// 대시 동안 0으로 바꾸므로 원래 값을 보관한다.
	float SavedGroundFriction = 0.f;
	float SavedBrakingDecelerationWalking = 0.f;


protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Blade")
	TSubclassOf<ABlade> BladeClass;

	/** 실제 월드에 생성된 환상검 객체를 가리키는 포인터 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Blade")
	TObjectPtr<ABlade> SpawnedBlade;

	/** 무기 소환 */
	void SpawnSpiritBlade();


protected:
	// 임시 : 기본 공격은 적중마다 같은 값을 준다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Combat|Damage")
	FCombatDamage BasicAttackDamage = FCombatDamage(25.f, 20.f);




};

