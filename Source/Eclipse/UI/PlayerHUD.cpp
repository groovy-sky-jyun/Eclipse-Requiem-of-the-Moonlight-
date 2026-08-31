// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerHUD.h"
#include "Eclipse.h"
#include "HealthBarWidget.h"
#include "PlayerCharacter.h"
#include "EnemyBoss.h"

void UPlayerHUD::NativeConstruct()
{
	Super::NativeConstruct();

	// 전투 전에는 보스 체력바가 자리를 차지하지 않는다.
	BossHealthBar->SetVisibility(ESlateVisibility::Collapsed);
}

void UPlayerHUD::BindPlayer(APlayerCharacter* Player)
{
	PlayerHealthBar->BindCharacter(Player);
}

void UPlayerHUD::ShowBossBar(AEnemyBoss* Boss)
{
	if (!IsValid(Boss))
	{
		UE_LOG(LogEclipse, Warning, TEXT("[HUD] ShowBossBar: invalid boss"));
		return;
	}

	BossHealthBar->BindCharacter(Boss);
	BossHealthBar->SetVisibility(ESlateVisibility::HitTestInvisible);

	OnBossBarShown();
}

void UPlayerHUD::HideBossBar()
{
	BossHealthBar->BindCharacter(nullptr);
	BossHealthBar->SetVisibility(ESlateVisibility::Collapsed);

	OnBossBarHidden();
}
