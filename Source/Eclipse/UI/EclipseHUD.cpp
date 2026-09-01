// Fill out your copyright notice in the Description page of Project Settings.

#include "EclipseHUD.h"
#include "Eclipse.h"
#include "PlayerHUD.h"
#include "BattleResultWidget.h"
#include "PlayerCharacter.h"
#include "GameFramework/PlayerController.h"

void AEclipseHUD::BeginPlay()
{
	Super::BeginPlay();

	if (!PlayerHUDClass)
	{
		UE_LOG(LogEclipse, Error, TEXT("[HUD] PlayerHUDClass is not set"));
		return;
	}

	PlayerHUD = CreateWidget<UPlayerHUD>(PlayerOwner, PlayerHUDClass);
	if (!PlayerHUD)
	{
		UE_LOG(LogEclipse, Error, TEXT("[HUD] Failed to create PlayerHUD"));
		return;
	}

	PlayerHUD->AddToViewport();

	if (PlayerOwner)
	{
		PlayerOwner->OnPossessedPawnChanged.AddDynamic(this, &AEclipseHUD::HandlePossessedPawnChanged);
		PlayerHUD->BindPlayer(Cast<APlayerCharacter>(PlayerOwner->GetPawn()));
	}

	if (AEclipseGameMode* GameMode = AEclipseGameMode::Get(this))
	{
		GameMode->OnBattleStarted.AddDynamic(this, &AEclipseHUD::HandleBattleStarted);
		GameMode->OnBattleFinished.AddDynamic(this, &AEclipseHUD::HandleBattleFinished);
	}
}

void AEclipseHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (PlayerOwner)
	{
		PlayerOwner->OnPossessedPawnChanged.RemoveDynamic(this, &AEclipseHUD::HandlePossessedPawnChanged);
	}

	if (AEclipseGameMode* GameMode = AEclipseGameMode::Get(this))
	{
		GameMode->OnBattleStarted.RemoveDynamic(this, &AEclipseHUD::HandleBattleStarted);
		GameMode->OnBattleFinished.RemoveDynamic(this, &AEclipseHUD::HandleBattleFinished);
	}

	// 뷰포트가 위젯 참조를 들고 있다. 직접 떼어내야 자식 위젯까지 정리된다.
	if (PlayerHUD)
	{
		PlayerHUD->RemoveFromParent();
		PlayerHUD = nullptr;
	}

	if (ResultWidget)
	{
		ResultWidget->RemoveFromParent();
		ResultWidget = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

void AEclipseHUD::HandleBattleStarted(AEnemyBoss* Boss)
{
	if (PlayerHUD)
	{
		PlayerHUD->ShowBossBar(Boss);
	}
}

void AEclipseHUD::HandleBattleFinished(EBattleResult Result)
{
	// 결과 화면이 전체를 덮는다. 루트만 접으면 체력바와 스킬 아이콘까지 함께 사라진다.
	if (PlayerHUD)
	{
		PlayerHUD->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (!ResultWidgetClass)
	{
		UE_LOG(LogEclipse, Error, TEXT("[HUD] ResultWidgetClass is not set"));
		return;
	}

	ResultWidget = CreateWidget<UBattleResultWidget>(PlayerOwner, ResultWidgetClass);
	if (!ResultWidget)
	{
		UE_LOG(LogEclipse, Error, TEXT("[HUD] Failed to create ResultWidget"));
		return;
	}

	ResultWidget->AddToViewport();
	ResultWidget->ShowResult(Result);

	// FreezeGameplay는 입력만 막는다. 버튼을 누르려면 커서를 직접 켜야 한다.
	if (PlayerOwner)
	{
		PlayerOwner->SetShowMouseCursor(true);
		PlayerOwner->SetInputMode(FInputModeUIOnly());
	}
}

void AEclipseHUD::HandlePossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	if (PlayerHUD)
	{
		PlayerHUD->BindPlayer(Cast<APlayerCharacter>(NewPawn));
	}
}
