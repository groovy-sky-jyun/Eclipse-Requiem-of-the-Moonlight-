// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "BaseCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Blade.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

APlayerCharacter::APlayerCharacter()
{
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
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();	

	// Enhanced Input Mapping Context 추가
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}

	SpawnSpiritBlade();
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
		EnhancedInputComponent->BindAction(IA_Dash, ETriggerEvent::Triggered, this, &APlayerCharacter::Dash);

		// PrimaryAttack
		EnhancedInputComponent->BindAction(IA_PrimaryAttack, ETriggerEvent::Triggered, this, &APlayerCharacter::PrimaryAttack);

		// Defense
		EnhancedInputComponent->BindAction(IA_Defense, ETriggerEvent::Started, this, &APlayerCharacter::DefenseStart);
		EnhancedInputComponent->BindAction(IA_Defense, ETriggerEvent::Completed, this, &APlayerCharacter::DefenseEnd);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void APlayerCharacter::JumpStart(const FInputActionValue& Value)
{
	Jump();
}

void APlayerCharacter::JumpEnd(const FInputActionValue& Value)
{
	StopJumping();
}

void APlayerCharacter::Dash(const FInputActionValue& Value)
{
	DoDash();
}

void APlayerCharacter::PrimaryAttack(const FInputActionValue& Value)
{
	bool bIsPressed = Value.Get<bool>();
	if (!bIsPressed) return;

	if (!SpawnedBlade) return;

	if (SpawnedBlade->GetCurrentState() != EBladeState::Idle) return;
	DoPrimaryAttack();
}

void APlayerCharacter::DefenseStart(const FInputActionValue& Value)
{
}

void APlayerCharacter::DefenseEnd(const FInputActionValue& Value)
{
}

void APlayerCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void APlayerCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void APlayerCharacter::DoDash()
{
	UE_LOG(LogTemp, Error, TEXT("Dash!!!"));
}

void APlayerCharacter::DoPrimaryAttack()
{
	// 1. 탐색 범위 설정 (정면 1500cm)
	FVector TraceStart = GetActorLocation();
	FVector TraceEnd = TraceStart + (GetActorForwardVector() * 1500.f);

	// 2. Multi Sweep 준비 (Pawn 타입 모두 찾기)
	TArray<FHitResult> HitResults;
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

	FCollisionQueryParams SweepParams;
	SweepParams.AddIgnoredActor(this);
	SweepParams.AddIgnoredActor(SpawnedBlade); // 내 캐릭터와 검은 탐색 제외

	// 3. 전방 넓은 구체 탐색 실행 (반경 150cm)
	bool bHit = GetWorld()->SweepMultiByObjectType(
		HitResults,
		TraceStart,
		TraceEnd,
		FQuat::Identity,
		ObjectQueryParams,
		FCollisionShape::MakeSphere(150.f),
		SweepParams
	);

	FVector FinalTarget = TraceEnd; // 기본 타겟은 허공

	if (bHit)
	{
		// 5. 스캔된 대상들을 가까운 순서대로 검증
		for (const FHitResult& Hit : HitResults)
		{
			AActor* TargetActor = Hit.GetActor();
			if (!TargetActor) continue;

			// [검증 1] 인터페이스 확인: 전투 가능한 객체인가?
			if (!TargetActor->Implements<UCombatInterface>()) continue;

			// [검증 2] 태그 확인: 진짜 '적(Enemy)'인가?
			// (적 블루프린트 액터의 Tags 배열에 "Enemy"가 추가되어 있어야 함)
			if (!TargetActor->ActorHasTag(FName("Team.Enemy"))) continue;

			// [검증 3] 시야 체크 (Line of Sight): 가림막이 없는가?
			FHitResult LoSHitResult;
			FCollisionQueryParams LoSParams;
			LoSParams.AddIgnoredActor(this);
			LoSParams.AddIgnoredActor(SpawnedBlade);

			// 플레이어 중심에서 적의 중심으로 얇은 레이저 발사
			bool bIsBlocked = GetWorld()->LineTraceSingleByChannel(
				LoSHitResult,
				GetActorLocation(),
				TargetActor->GetActorLocation(),
				ECC_Visibility, // 시야를 가리는 물체(벽, 튼튼한 울타리 등) 감지
				LoSParams
			);

			// 레이저가 아무것에도 안 막혔거나, 막힌 물체가 바로 그 적이라면 통과!
			if (!bIsBlocked || LoSHitResult.GetActor() == TargetActor)
			{
				// 완벽한 타겟 확정
				FinalTarget = TargetActor->GetActorLocation();

				// 캐릭터 몸통을 적 방향으로 회전 (위아래 꺾임 방지)
				FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), FinalTarget);
				SetActorRotation(FRotator(0.f, LookAtRot.Yaw, 0.f));

				// 가장 가까운 유효 타겟을 찾았으니 탐색 루프 즉시 종료
				break;
			}
		}
	}

	// 6. 검 발사
	SpawnedBlade->Launch(FinalTarget);
}

void APlayerCharacter::DoDefenseStart()
{
}

void APlayerCharacter::DoDefenseEnd()
{
}

void APlayerCharacter::HandleTakeDamage_Implementation(float DamageAmount, AActor* Attacker)
{
	Super::HandleTakeDamage_Implementation(DamageAmount, Attacker);


}

void APlayerCharacter::Die_Implementation()
{
	Super::Die_Implementation();

	UE_LOG(LogTemp, Error, TEXT("---Game Over---"));
}

void APlayerCharacter::SpawnSpiritBlade()
{
	if (!BladeClass) return;

	UWorld* World = GetWorld();
	if (World)
	{
		// 1. 소환 위치 및 방향 설정
		FVector TargetLocation = GetActorLocation() + GetActorRotation().RotateVector(FVector(-100.f, -50.f, 50.f));
		FRotator SpawnRotation = GetActorRotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		// 2. Blade Spawn
		SpawnedBlade = World->SpawnActor<ABlade>(BladeClass, TargetLocation, SpawnRotation, SpawnParams);
	}
}

