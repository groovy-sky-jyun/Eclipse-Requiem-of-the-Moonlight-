// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "EclipseGameMode.h"   // EBattleResult
#include "EclipseHUD.generated.h"

class UPlayerHUD;
class AEnemyBoss;

/**
 *  화면 UI의 소유자. 언제 무엇을 띄우는지 관리
 */
UCLASS(abstract)
class ECLIPSE_API AEclipseHUD : public AHUD
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "UI")
	UPlayerHUD* GetPlayerHUD() const { return PlayerHUD; }


protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void HandleBattleStarted(AEnemyBoss* Boss);

	UFUNCTION()
	void HandleBattleFinished(EBattleResult Result);

	/** HUD가 폰 소유보다 먼저 만들어질 수 있어 폰 교체도 함께 본다. */
	UFUNCTION()
	void HandlePossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);


protected:
	/** 게임 시작부터 끝까지 화면에 유지되는 루트 위젯 */
	UPROPERTY(EditDefaultsOnly, Category = "Settings|UI")
	TSubclassOf<UPlayerHUD> PlayerHUDClass;

	UPROPERTY(Transient)
	TObjectPtr<UPlayerHUD> PlayerHUD;
};
