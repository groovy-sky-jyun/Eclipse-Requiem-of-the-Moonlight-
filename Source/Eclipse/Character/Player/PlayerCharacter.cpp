// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "BaseCharacter.h"
#include "Engine/OverlapResult.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Blade.h"
#include "CombatInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "GameplayTagContainer.h"
#include "EclipseGameMode.h"

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

		// Attack
		EnhancedInputComponent->BindAction(IA_Attack, ETriggerEvent::Triggered, this, &APlayerCharacter::BasicAttack);
		EnhancedInputComponent->BindAction(IA_FirstSpecialAttack, ETriggerEvent::Triggered, this, &APlayerCharacter::FirstSpecialAttack);
		EnhancedInputComponent->BindAction(IA_SecondSpecialAttack, ETriggerEvent::Triggered, this, &APlayerCharacter::SecondSpecialAttack);
		EnhancedInputComponent->BindAction(IA_UltimateAttack, ETriggerEvent::Triggered, this, &APlayerCharacter::UltimateAttack);

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
	DoMove(Value.Get<FVector2D>().X, Value.Get<FVector2D>().Y);
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	DoLook(Value.Get<FVector2D>().X, Value.Get<FVector2D>().Y);
}

void APlayerCharacter::JumpStart(const FInputActionValue& Value) {	Jump(); }
void APlayerCharacter::JumpEnd(const FInputActionValue& Value) { StopJumping(); }
void APlayerCharacter::Dash(const FInputActionValue& Value) { DoDash(); }

void APlayerCharacter::BasicAttack(const FInputActionValue& Value)
{
	if (!Value.Get<bool>()) return;
	if (!SpawnedBlade) return;
	if (SpawnedBlade->GetCurrentState() != EBladeState::Idle)
	{
		UE_LOG(LogTemp, Warning, TEXT("Blade State is not Idle. Can't Attack"));
		return;
	}
	DoBasicAttack();
}

void APlayerCharacter::FirstSpecialAttack(const FInputActionValue& Value)
{
}

void APlayerCharacter::SecondSpecialAttack(const FInputActionValue& Value)
{
}

void APlayerCharacter::UltimateAttack(const FInputActionValue& Value)
{
}

void APlayerCharacter::DefenseStart(const FInputActionValue& Value) { DoDefenseStart(); }
void APlayerCharacter::DefenseEnd(const FInputActionValue& Value) { DoDefenseEnd(); }

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
	AddControllerYawInput(Yaw);
	AddControllerPitchInput(Pitch);
}

void APlayerCharacter::DoDash()
{
	FVector ForwardDir = GetActorForwardVector();
	LaunchCharacter(GetActorForwardVector() * DashDistance, true, true);
}

void APlayerCharacter::DoBasicAttack()
{
	// 1. 화면 중앙 좌표(Screen Center) 구하기
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) return;

	int32 ViewportSizeX, ViewportSizeY;
	PC->GetViewportSize(ViewportSizeX, ViewportSizeY);
	FVector2D ScreenCenter(ViewportSizeX * 0.5f, ViewportSizeY * 0.5f);


	// 2. 플레이어 주변 반경 안에 있는 모든 Pawn 검색
	FVector PlayerLocation = GetActorLocation();
	TArray<FOverlapResult> OverlapResults;
	FCollisionObjectQueryParams ObjectQueryParams(ECC_Pawn);
	FCollisionQueryParams SweepParams;
	SweepParams.AddIgnoredActor(this);
	SweepParams.AddIgnoredActor(SpawnedBlade);

	GetWorld()->OverlapMultiByObjectType(
		OverlapResults,
		PlayerLocation,
		FQuat::Identity,
		ObjectQueryParams,
		FCollisionShape::MakeSphere(700.f),
		SweepParams
	);

	float DebugTime = 2.0f; // 화면에 그려져 있을 시간(초)
	DrawDebugSphere(GetWorld(), PlayerLocation, 700.f, 24, FColor::Yellow, false, DebugTime);

	struct FTargetCandidate
	{
		AActor* TargetActor;
		float TargetScore;
	};

	TArray<FTargetCandidate> Candidates;
	TSet<AActor*> ProcessedActors;

	// 4. 후보군 필터링 및 2D 거리 계산
	for (const FOverlapResult& Overlap : OverlapResults)
	{
		AActor* TargetActor = Overlap.GetActor();
		if (!TargetActor) continue;

		if (ProcessedActors.Contains(TargetActor)) continue;
		ProcessedActors.Add(TargetActor);

		ICombatInterface* CombatTarget = Cast<ICombatInterface>(TargetActor);
		if (!CombatTarget) continue;
	
		FGameplayTag TargetTeamTag = ICombatInterface::Execute_GetTeamTag(TargetActor);
		if (!TargetTeamTag.MatchesTag(EnemyTag)) continue;

		FVector2D ScreenPos;
		bool bIsOnScreen = PC->ProjectWorldLocationToScreen(TargetActor->GetActorLocation(), ScreenPos);

		if (bIsOnScreen)
		{
			// 화면 중앙으로부터의 거리 (2D)
			float DistToCenter = FVector2D::Distance(ScreenCenter, ScreenPos);
			// 캐릭터로부터의 실제 거리 (3D)
			float DistToPlayer = FVector::Dist(GetActorLocation(), TargetActor->GetActorLocation());
			// 점수 매기기 (값이 작을수록 타겟)
			float TargetScore = (DistToCenter * 0.7f) + (DistToPlayer * 0.3f);

			Candidates.Add({ TargetActor, TargetScore });
			//UE_LOG(LogTemp, Warning, TEXT("OverlapActor Name is %s"), *TargetActor->GetActorNameOrLabel());
		}
	}

	// 5. 화면 중앙에 가장 가까운 순서대로 배열 오름차순 정렬
	Candidates.Sort([](const FTargetCandidate& A, const FTargetCandidate& B) {
		return A.TargetScore < B.TargetScore;
		});

	FVector FinalTarget = FVector::ZeroVector;
	AActor* BestTargetActor = nullptr;

	// 6. TargetScore 낮은놈부터 '시야(벽) 체크'
	for (const FTargetCandidate& Candidate : Candidates)
	{
		FHitResult LoSHitResult;
		FCollisionQueryParams LoSParams;
		LoSParams.AddIgnoredActor(this);
		LoSParams.AddIgnoredActor(SpawnedBlade);

		bool bIsBlocked = GetWorld()->LineTraceSingleByChannel(
			LoSHitResult,
			GetActorLocation(),
			Candidate.TargetActor->GetActorLocation(),
			ECC_Visibility,
			LoSParams
		);

		if (!bIsBlocked || LoSHitResult.GetActor() == Candidate.TargetActor)
		{
			DrawDebugLine(GetWorld(), GetActorLocation(), Candidate.TargetActor->GetActorLocation(), FColor::Green, false, DebugTime, 0, 2.0f);
			DrawDebugSphere(GetWorld(), Candidate.TargetActor->GetActorLocation(), 20.f, 12, FColor::Green, false, DebugTime);

			BestTargetActor = Candidate.TargetActor;
			FinalTarget = BestTargetActor->GetActorLocation();
			break;
		}
		else
		{
			DrawDebugLine(GetWorld(), GetActorLocation(), LoSHitResult.ImpactPoint, FColor::Red, false, DebugTime, 0, 1.0f);
			DrawDebugBox(GetWorld(), LoSHitResult.ImpactPoint, FVector(10.f), FQuat::Identity, FColor::Red, false, DebugTime);
		}
	}

	// 7. 발사 처리 및 방향 보정
	if (BestTargetActor)
	{
		// 적을 찾았다면: 캐릭터의 몸을 적 방향으로 회전 후 발사
		FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), FinalTarget);
		SetActorRotation(FRotator(0.f, LookAtRot.Yaw, 0.f));

		DrawDebugSphere(GetWorld(), FinalTarget, 30.f, 12, FColor::Red, false, 2.0f);
	}
	else
	{
		// 화면에 적이 없거나 다 벽에 가려졌다면: 카메라가 바라보는 정면 허공으로 발사
		FVector CameraForward = PC->GetControlRotation().Vector();
		FinalTarget = GetActorLocation() + (CameraForward * 1500.f);
	}

	// 무기에게 좌표 던져주기
	if (ComboIndex >= ComboDamageList.Num())
	{
		ResetCombo();
	}

	SpawnedBlade->SetBladeDamage(ComboDamageList[ComboIndex]);
	SpawnedBlade->Launch(FinalTarget);
}

void APlayerCharacter::DoFirstSpecialAttack()
{
}

void APlayerCharacter::DoSecondSpecialAttack()
{
}

void APlayerCharacter::DoUltimateAttack()
{
}

void APlayerCharacter::ResetCombo()
{
	if (ComboIndex != 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Combo] 타이머 만료 → 콤보 리셋 (이전 인덱스: %d)"), ComboIndex);
		ComboIndex = 0;
	}
}

void APlayerCharacter::UpdateBasicCombo()
{
	ComboIndex++;
	if (ComboIndex >= MaxComboIndexNum)
	{
		ResetCombo();
	}

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

	if (AEclipseGameMode* GameMode = AEclipseGameMode::Get(this))
	{
		GameMode->NotifyPlayerDied();
	}
}

void APlayerCharacter::SpawnSpiritBlade()
{
	if (!BladeClass) return;

	UWorld* World = GetWorld();
	if (!World) return;

	const FVector SpawnLoc = GetActorLocation() + GetActorRotation().RotateVector(FVector(-100.f, -50.f, 50.f));

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	SpawnedBlade = World->SpawnActor<ABlade>(BladeClass, SpawnLoc, GetActorRotation(), SpawnParams);
}

