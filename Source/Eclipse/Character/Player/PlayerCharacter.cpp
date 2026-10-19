// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Eclipse.h"
#include "BaseCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "StaminaComponent.h"
#include "PlayerAttackComponent.h"
#include "DashComponent.h"
#include "CombatInterface.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameplayTagContainer.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyBoss.h"

APlayerCharacter::APlayerCharacter()
{
	// 카메라 보정을 매 틱 갱신한다.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	StaminaComponent = CreateDefaultSubobject<UStaminaComponent>(TEXT("StaminaComponent"));
	AttackComponent = CreateDefaultSubobject<UPlayerAttackComponent>(TEXT("AttackComponent"));
	DashComponent = CreateDefaultSubobject<UDashComponent>(TEXT("DashComponent"));

	MaxHealth = 1000;

	HitReactionData.Add(EHitIntensity::Light, EHitReaction::None);
	HitReactionData.Add(EHitIntensity::Medium, EHitReaction::None);
	HitReactionData.Add(EHitIntensity::Heavy, EHitReaction::Knockback);
	HitReactionData.Add(EHitIntensity::Massive, EHitReaction::Knockdown);
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateCameraAssist(DeltaSeconds);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Jumping
		EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Started, this, &APlayerCharacter::JumpStart);
		EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Completed, this, &APlayerCharacter::JumpEnd);

		// Moving
		EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(IA_Look, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);

		// Dash
		// 누르고 있는 동안 매 프레임 대시가 나가지 않도록 Started로 받는다.
		EnhancedInputComponent->BindAction(IA_Dash, ETriggerEvent::Started, this, &APlayerCharacter::Dash);

		// Attack
		// 누르고 있는 동안 매 프레임 호출되지 않도록 Started로 받는다. 입력 태그를 함께 넘긴다.
		for (const FAttackInputMapping& Mapping : AttackInputMappings)
		{
			if (!Mapping.InputAction || !Mapping.InputTag.IsValid())
			{
				UE_LOG(LogPlayerAttack, Warning, TEXT("[Player] Attack input mapping has empty action or tag"));
				continue;
			}

			EnhancedInputComponent->BindAction(Mapping.InputAction, ETriggerEvent::Started, this, &APlayerCharacter::AttackInput, Mapping.InputTag);
		}
	}
	else
	{
		UE_LOG(LogEclipse, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	DoMove(Value.Get<FVector2D>().X, Value.Get<FVector2D>().Y);
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	DoLook(Value.Get<FVector2D>().X, Value.Get<FVector2D>().Y);
}

void APlayerCharacter::JumpStart(const FInputActionValue& Value) 
{	
	if (IsDead_Implementation()) return;
	if (!AttackComponent->CanCancelAttack()) return;
	if (!CanJump()) return;

	AttackComponent->CancelAttack();
	Jump(); 
}

void APlayerCharacter::JumpEnd(const FInputActionValue& Value) { StopJumping(); }

void APlayerCharacter::Dash(const FInputActionValue& Value)
{
	if (TryBufferHitAction(EHitBufferedAction::Dash)) return;

	DoDash();
}

void APlayerCharacter::DoDash()
{
	if (IsDead_Implementation()) return;
	if (!AttackComponent->CanCancelAttack()) return;
	if (!DashComponent->CanDash()) return;

	AttackComponent->CancelAttack();
	DashComponent->StartDash();
}

void APlayerCharacter::AttackInput(FGameplayTag InputTag)
{
	UE_LOG(LogPlayerAttack, Log, TEXT("[Player] Attack input : %s"), *InputTag.ToString());
	if (TryBufferHitAction(EHitBufferedAction::Attack, InputTag)) return;

	AttackComponent->RequestAttack(InputTag);
}


void APlayerCharacter::DoMove(float Right, float Forward)
{
	if (!GetController()) return;
	
	// find out which way is forward
	const FRotator YawRotation(0, GetController()->GetControlRotation().Yaw, 0);

	// add movement 
	AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), Forward);
	AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y), Right);
}

void APlayerCharacter::DoLook(float Yaw, float Pitch)
{
	if (!GetController()) return;

	// 플레이어가 마우스 움직이면 방향 보정을 멈춘다.
	if (!FMath::IsNearlyZero(Yaw) || !FMath::IsNearlyZero(Pitch))
	{
		bIsCameraAssisting = false;
	}

	AddControllerYawInput(Yaw);
	AddControllerPitchInput(Pitch);
}

void APlayerCharacter::OnDamaged(const FCombatDamage& DamageInfo, AActor* Attacker)
{
	Super::OnDamaged(DamageInfo, Attacker);

	EHitReaction NewReaction = HitReactionData.FindRef(DamageInfo.HitIntensity);
	switch (NewReaction)
	{
	case EHitReaction::None:
		OnHit();
		break;

	case EHitReaction::Knockback:
		OnKnockback();
		break;

	case EHitReaction::Knockdown:
		OnKnockdown();
		break;
	}
}

void APlayerCharacter::OnDeath()
{
	Super::OnDeath();
}

void APlayerCharacter::OnHit()
{
}

void APlayerCharacter::OnKnockback()
{
	EnterHitReactionLock(KnockbackMontage);
}

void APlayerCharacter::OnKnockdown()
{
	EnterHitReactionLock(KnockdownMontage);
}

void APlayerCharacter::EnterHitReactionLock(UAnimMontage* Montage)
{
	HitRecoveryPhase = EHitRecoveryPhase::Locked;
	BufferedHitAction = EHitBufferedAction::None;

	// 피격 몽타주가 공격 몽타주를 끊기 전에 공격을 정리한다.
	AttackComponent->CancelAttack();

	UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	if (!Montage || !AnimInstance || AnimInstance->Montage_Play(Montage) <= 0.f)
	{
		UE_LOG(LogEclipse, Warning, TEXT("[%s] Hit reaction montage not played"), *GetName());
		HitRecoveryPhase = EHitRecoveryPhase::None;
		return;
	}

	FOnMontageBlendingOutStarted BlendingOutDelegate = FOnMontageBlendingOutStarted::CreateUObject(this, &APlayerCharacter::HandleHitReactionMontageBlendingOut);
	AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, Montage);
}

void APlayerCharacter::ExitHitReactionLock()
{
	if (HitRecoveryPhase == EHitRecoveryPhase::None) return;

	HitRecoveryPhase = EHitRecoveryPhase::None;
	ExecuteBufferedHitAction();
}

void APlayerCharacter::HandleHitReactionMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	// 끊긴 경우(새 피격, 회복 후 행동)는 끊은 쪽에서 이미 상태를 정했다.
	if (bInterrupted) return;

	ExitHitReactionLock();
}

void APlayerCharacter::NotifyHitRecoveryBegin()
{
	if (HitRecoveryPhase != EHitRecoveryPhase::Locked) return;

	HitRecoveryPhase = EHitRecoveryPhase::Buffering;
}

void APlayerCharacter::NotifyHitRecoveryEnd()
{
	// 새 피격으로 Locked가 된 뒤 이전 몽타주의 끝 알림이 오면 무시한다.
	if (HitRecoveryPhase != EHitRecoveryPhase::Buffering) return;

	ExitHitReactionLock();
}

bool APlayerCharacter::TryBufferHitAction(EHitBufferedAction Action, const FGameplayTag& AttackTag)
{
	switch (HitRecoveryPhase)
	{
	case EHitRecoveryPhase::Locked:
		return true;

	case EHitRecoveryPhase::Buffering:
		// 마지막 입력만 남긴다.
		BufferedHitAction = Action;
		BufferedAttackTag = AttackTag;
		return true;

	default:
		return false;
	}
}

void APlayerCharacter::ExecuteBufferedHitAction()
{
	const EHitBufferedAction Action = BufferedHitAction;
	BufferedHitAction = EHitBufferedAction::None;

	switch (Action)
	{
	case EHitBufferedAction::Attack:
		AttackComponent->RequestAttack(BufferedAttackTag);
		break;

	case EHitBufferedAction::Dash:
		DoDash();
		break;

	default:
		break;
	}
}

void APlayerCharacter::FaceAttackTarget()
{
	AActor* Target = GetAttackTarget();
	if (!Target) return; 

	FVector ToTarget = Target->GetActorLocation() - GetActorLocation();
	ToTarget.Z = 0.f;

	if (ToTarget.IsNearlyZero()) return;
	if (ToTarget.SizeSquared() > FMath::Square(FaceTargetMaxDistance)) return;

	const float TargetYaw = ToTarget.Rotation().Yaw;
	SetActorRotation(FRotator(0.f, TargetYaw, 0.f)); // 1. 캐릭터 몸 회전 (무조건)

	StartCameraAssist(TargetYaw); // 2. 카메라 보정 (조건에 따라)
}

AActor* APlayerCharacter::GetAttackTarget()
{
	// 몬스터가 하나뿐이기에 없으면 World에서 찾는다.
	if (!AttackTarget.IsValid())
	{
		AttackTarget = UGameplayStatics::GetActorOfClass(GetWorld(), AEnemyBoss::StaticClass());
	}

	AActor* Target = AttackTarget.Get();
	if (!Target || ICombatInterface::Execute_IsDead(Target)) return nullptr;

	return Target;
}

void APlayerCharacter::StartCameraAssist(float TargetYaw)
{
	if (!Controller) return;

	const float DeltaYaw = FMath::FindDeltaAngleDegrees(TargetYaw, Controller->GetControlRotation().Yaw);

	// 보스가 이미 화면 안쪽이면 카메라 보정 x
	if (FMath::Abs(DeltaYaw) < CameraAssistAngle) return;

	// 카메라 보정 방향을 살짝 대각선으로 설정
	const float Side = DeltaYaw >= 0.f ? 1.f : -1.f;
	CameraAssistTargetYaw = TargetYaw + Side * CameraAssistOffsetYaw;
	bIsCameraAssisting = true;
}

void APlayerCharacter::UpdateCameraAssist(float DeltaSeconds)
{
	if (!bIsCameraAssisting || !Controller) return;

	const FRotator Current = Controller->GetControlRotation();
	const FRotator Target(Current.Pitch, CameraAssistTargetYaw, Current.Roll);
	const FRotator NewRotation = FMath::RInterpTo(Current, Target, DeltaSeconds, CameraAssistSpeed);
	Controller->SetControlRotation(NewRotation);

	if (FMath::Abs(FMath::FindDeltaAngleDegrees(NewRotation.Yaw, CameraAssistTargetYaw)) < 1.f)
	{
		bIsCameraAssisting = false;
	}
}
