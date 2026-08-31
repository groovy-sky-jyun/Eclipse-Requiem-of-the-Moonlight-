// Fill out your copyright notice in the Description page of Project Settings.

#include "EclipseHUD.h"
#include "Eclipse.h"
#include "PlayerHUD.h"
#include "PlayerCharacter.h"

void AEclipseHUD::BeginPlay()
{
	Super::BeginPlay();

	if (!PlayerHUDClass)
	{
		UE_LOG(LogEclipse, Warning, TEXT("[HUD] PlayerHUDClass is not set"));
		return;
	}

	PlayerHUD = CreateWidget<UPlayerHUD>(PlayerOwner, PlayerHUDClass);
	if (!PlayerHUD)
	{
		UE_LOG(LogEclipse, Warning, TEXT("[HUD] Failed to create PlayerHUD"));
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
	if (PlayerHUD)
	{
		PlayerHUD->HideBossBar();
	}
}

void AEclipseHUD::HandlePossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	if (PlayerHUD)
	{
		PlayerHUD->BindPlayer(Cast<APlayerCharacter>(NewPawn));
	}
}
