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
class UPlayerAttackComponent;
class UDashComponent;

/** 피격 액션. */
UENUM(BlueprintType)
enum class EHitReaction : uint8
{
	// VFX, Sound 등 연출만 적용
	None,
	// 몸 전체가 공격받은 방향의 뒤로 살짤 밀려난다. + 짧은 입력 잠금
	Knockback,
	// 다운(엉덩방아) -> 기상(무적) + 입력잠금
	Knockdown
};

/** 피격 중 입력 처리 단계. */
UENUM(BlueprintType)
enum class EHitRecoveryPhase : uint8
{
	// 피격 아님. 입력 즉시 실행
	None,
	// 입력 무시
	Locked,
	// 입력 저장 후 회복 시점에 실행
	Buffering
};

/** 회복 구간에 저장하는 입력. */
UENUM(BlueprintType)
enum class EHitBufferedAction : uint8
{
	None,
	Attack,
	Dash
};

/** 공격 입력 액션과 입력 태그 매칭. */
USTRUCT(BlueprintType)
struct FAttackInputMapping
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> InputAction;

	UPROPERTY(EditAnywhere, meta = (Categories = "Input"))
	FGameplayTag InputTag;
};

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
	// 임시 : 환상검 기본 공격. 연결 해제 상태, 추후 정리
	void BasicAttack(const FInputActionValue& Value);

	/** 모든 공격 입력이 여기로 온다. 어떤 입력인지는 태그로 구분한다. */
	void AttackInput(FGameplayTag InputTag);


public:
	UFUNCTION(Category = "Input")
	virtual void DoMove(float Right, float Forward);

	UFUNCTION(Category = "Input")
	virtual void DoLook(float Yaw, float Pitch);

	// 임시 : 환상검 기본 공격. 연결 해제 상태, 추후 정리
	UFUNCTION(Category = "Input")
	void DoBasicAttack();

	void DoDash();

	// 회복 구간 시작. 이후 입력은 저장한다.
	void NotifyHitRecoveryBegin();

	// 회복 시점. 저장된 입력을 실행하고 이후 입력은 즉시 실행한다.
	void NotifyHitRecoveryEnd();


protected:
	virtual void OnDamaged(const FCombatDamage& DamageInfo, AActor* Attacker) override;

	virtual void OnDeath() override;

	void OnHit();

	void OnKnockback();

	void OnKnockdown();

	/** 몽타주 재생 + 입력 잠금. Hit Recovery Window 끝이나 몽타주 종료에서 복귀한다. */
	void EnterHitReactionLock(UAnimMontage* Montage);

	void ExitHitReactionLock();

	// 회복 구간이 없는 몽타주도 끝나면 복귀하도록 한다.
	void HandleHitReactionMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);

	/** 피격 중이면 입력을 무시하거나 저장한다. 처리했으면 true. */
	bool TryBufferHitAction(EHitBufferedAction Action, const FGameplayTag& AttackTag = FGameplayTag());

	void ExecuteBufferedHitAction();

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

	// 공격 입력 액션 -> 입력 태그. 새 공격 입력은 함수 추가 없이 여기에 추가한다.
	UPROPERTY(EditAnywhere, Category = "Settings|Input", meta = (TitleProperty = "InputTag"))
	TArray<FAttackInputMapping> AttackInputMappings;


public:
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(BlueprintPure, Category = "Stamina")
	UStaminaComponent* GetStaminaComponent() const { return StaminaComponent; }

	UFUNCTION(BlueprintPure, Category = "Combat")
	UPlayerAttackComponent* GetAttackComponent() const { return AttackComponent; }

	UFUNCTION(BlueprintPure, Category = "Dash")
	UDashComponent* GetDashComponent() const { return DashComponent; }


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaminaComponent> StaminaComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UPlayerAttackComponent> AttackComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UDashComponent> DashComponent;


protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Blade")
	TSubclassOf<ABlade> BladeClass;

	/** 실제 월드에 생성된 환상검 객체를 가리키는 포인터 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Blade")
	TObjectPtr<ABlade> SpawnedBlade;

	/** 무기 소환 */
	void SpawnSpiritBlade();


protected:
	/** 공격 등급과 피격 액션 매칭 데이터 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Combat|Damage")
	TMap<EHitIntensity, EHitReaction> HitReactionData;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|HitReaction")
	TObjectPtr<UAnimMontage> KnockbackMontage;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|HitReaction")
	TObjectPtr<UAnimMontage> KnockdownMontage;

	EHitRecoveryPhase HitRecoveryPhase = EHitRecoveryPhase::None;

	EHitBufferedAction BufferedHitAction = EHitBufferedAction::None;

	FGameplayTag BufferedAttackTag;

	// 임시 : 기본 공격은 적중마다 같은 값을 준다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Combat|Damage")
	FCombatDamage BasicAttackDamage = FCombatDamage(25.f, 20.f);




};

