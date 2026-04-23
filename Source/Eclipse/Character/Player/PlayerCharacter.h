// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;
class UInputMappingContext;
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

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	void JumpStart(const FInputActionValue& Value);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	void JumpEnd(const FInputActionValue& Value);

	void Dash(const FInputActionValue& Value);

	void PrimaryAttack(const FInputActionValue& Value);

	void DefenseStart(const FInputActionValue& Value);
	void DefenseEnd(const FInputActionValue& Value);


public:
	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoLook(float Yaw, float Pitch);

	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoDash();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoPrimaryAttack();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoDefenseStart();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoDefenseEnd();

	virtual void HandleTakeDamage_Implementation(float DamageAmount, AActor* Attacker) override;
	virtual void Die_Implementation() override;


protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category = "Settings|Input")
	UInputAction* IA_Move;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category = "Settings|Input")
	UInputAction* IA_Look;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category = "Settings|Input")
	UInputAction* IA_Jump;

	/** Dash Input Action */
	UPROPERTY(EditAnywhere, Category = "Settings|Input")
	UInputAction* IA_Dash;

	/** PrimaryAttack Input Action */
	UPROPERTY(EditAnywhere, Category = "Settings|Input")
	UInputAction* IA_PrimaryAttack;

	/** Defens Input Action */
	UPROPERTY(EditAnywhere, Category = "Settings|Input")
	UInputAction* IA_Defense;


public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }


protected:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;


protected:
	/** 소환할 환상검의 블루프린트 클래스 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Blade")
	TSubclassOf<ABlade> BladeClass;

	/** 실제 월드에 생성된 환상검 객체를 가리키는 포인터 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Blade")
	TObjectPtr<ABlade> SpawnedBlade;


protected:
	/** 검을 소환하는 함수 */
	void SpawnSpiritBlade();

};

