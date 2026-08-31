// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUD.generated.h"

class UHealthBarWidget;
class APlayerCharacter;
class AEnemyBoss;

/**
 *  게임 내내 화면에 떠 있는 루트 위젯.
 *  플레이어 체력바와 스킬 아이콘은 항상 보이고, 보스 체력바만 전투 중에 나타난다.
 */
UCLASS(abstract)
class ECLIPSE_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	void BindPlayer(APlayerCharacter* Player);

	/** 전투 시작. 보스 체력바를 대상에 연결하고 화면에 띄운다. */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowBossBar(AEnemyBoss* Boss);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideBossBar();


protected:
	virtual void NativeConstruct() override;

	/** 등장 / 퇴장 연출은 블루프린트 애니메이션으로 한다. */
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnBossBarShown();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnBossBarHidden();

	/** 위젯 블루프린트에 같은 이름의 자식 위젯이 있어야 한다. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UHealthBarWidget> PlayerHealthBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UHealthBarWidget> BossHealthBar;
};
