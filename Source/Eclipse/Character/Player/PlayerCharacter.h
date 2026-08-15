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

	virtual void HandleTakeDamage_Implementation(float DamageAmount, AActor* Attacker) override;
	virtual void Die_Implementation() override;


protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

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


public:
	FORCEINLINE int32 GetComboIndex() const { return ComboIndex; }
	FORCEINLINE int32 GetComboDamage(int32 Index) const { return ComboDamageList[Index]; }
	FORCEINLINE int32 GetMaxComboIndexNum() const { return MaxComboIndexNum; }
	/** 콤보를 처음으로 되돌리는 함수 (타이머 콜백) */
	void ResetCombo();

	void UpdateBasicCombo();


protected:
	/** 각 콤보 타수의 데미지(기본공격) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Combat|Combo")
	TArray<float> ComboDamageList = { 25.f, 40.f, 80.f };

	/**
	 * 마지막 공격 후 이 시간(초) 안에 다시 공격하지 않으면 콤보 리셋.
	 * 검이 돌아온 뒤 플레이어가 얼마나 빠르게 다시 클릭해야 하는지 결정.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Combat|Combo")
	float ComboResetDelay = 1.5f;

	/** 현재 콤보 인덱스 (0~2). 외부 노출은 Getter로만. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Combat|Combo")
	int32 ComboIndex = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Combat|Combo")
	int32 MaxComboIndexNum = 3;

	/** 콤보 자동 리셋 타이머 핸들 */
	FTimerHandle ComboResetTimerHandle;



protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Enemy|Tag")
	FGameplayTag EnemyTag;

};

