// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleResultWidget.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"

void UBattleResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	QuitButton->OnClicked.AddDynamic(this, &UBattleResultWidget::HandleQuitClicked);
	RetryButton->OnClicked.AddDynamic(this, &UBattleResultWidget::HandleRetryClicked);
}

void UBattleResultWidget::NativeDestruct()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(CountdownTimer);
	}

	Super::NativeDestruct();
}

void UBattleResultWidget::ShowResult(EBattleResult Result)
{
	OnResultShown(Result);

	GetWorld()->GetTimerManager().SetTimer(CountdownTimer, this, &UBattleResultWidget::UpdateCountdown, 1.f, true);
	UpdateCountdown();
}

void UBattleResultWidget::UpdateCountdown()
{
	AEclipseGameMode* GameMode = AEclipseGameMode::Get(this);
	const float Remaining = GameMode ? GameMode->GetRestartTimeRemaining() : -1.f;

	// 자동 재시작이 꺼져 있으면 문구를 비우고 갱신을 멈춘다.
	if (Remaining < 0.f)
	{
		GetWorld()->GetTimerManager().ClearTimer(CountdownTimer);
		OnCountdownUpdated(FText::GetEmpty());
		return;
	}

	const int32 Seconds = FMath::CeilToInt(Remaining);
	OnCountdownUpdated(FText::FromString(FString::Printf(TEXT("%02d:%02d"), Seconds / 60, Seconds % 60)));
}

void UBattleResultWidget::HandleQuitClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}

void UBattleResultWidget::HandleRetryClicked()
{
	if (AEclipseGameMode* GameMode = AEclipseGameMode::Get(this))
	{
		GameMode->RequestRestart();
	}
}
