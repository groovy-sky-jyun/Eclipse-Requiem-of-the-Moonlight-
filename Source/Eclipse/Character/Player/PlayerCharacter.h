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
	void DefenseStart(const FInputActionValue& Value);
	void DefenseEnd(const FInputActionValue& Value);


public:
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoMove(float Right, float Forward);

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoLook(float Yaw, float Pitch);

	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoDash();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoBasicAttack();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoFirstSpecialAttack();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoSecondSpecialAttack();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoUltimateAttack();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoDefenseStart();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoDefenseEnd();



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

	UPROPERTY(EditAnywhere, Category = "Settings|Input")
	TObjectPtr<UInputAction> IA_Defense;

	UPROPERTY(EditAnywhere, Category = "Setting|Input|Dash")
	float DashDistance = 2000.f;


public:
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;


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

