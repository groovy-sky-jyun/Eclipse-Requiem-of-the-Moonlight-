// Copyright Epic Games, Inc. All Rights Reserved.


#include "EclipsePlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"

void AEclipsePlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 뷰포트의 입력 모드는 레벨 전환 후에도 남는다. 결과 화면에서 켠 커서를 여기서 되돌린다.
	SetInputMode(FInputModeGameOnly());
	SetShowMouseCursor(false);
}

void AEclipsePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (!IsLocalPlayerController()) return;

	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (!Subsystem) return;

	for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
	{
		if (CurrentContext)
		{
			Subsystem->AddMappingContext(CurrentContext, 0);
		}
	}
}
