// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EclipseGameMode.h"   // EBattleResult
#include "BattleResultWidget.generated.h"

class UButton;

/**
 *  전투 종료 화면. 승리와 패배가 같은 구성이고 글자와 배경색만 다르다.
 *  자동 재시작까지 남은 시간은 GameMode의 타이머를 그대로 읽어 표시한다.
 */
UCLASS(abstract)
class ECLIPSE_API UBattleResultWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** 결과 화면을 띄우고 카운트다운을 시작한다. */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowResult(EBattleResult Result);


protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/** 큰 글자와 배경색은 블루프린트가 결과에 맞춰 정한다. */
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnResultShown(EBattleResult Result);

	/** 남은 시간을 "01:00" 형태로 넘긴다. 비어 있으면 자동 재시작이 꺼진 상태다. */
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnCountdownUpdated(const FText& RemainingTime);

	UFUNCTION()
	void HandleQuitClicked();

	UFUNCTION()
	void HandleRetryClicked();

	/** 위젯 블루프린트에 같은 이름의 버튼이 있어야 한다. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> QuitButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> RetryButton;


private:
	void UpdateCountdown();

	FTimerHandle CountdownTimer;
};
