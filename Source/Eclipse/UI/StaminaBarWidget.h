// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StaminaBarWidget.generated.h"

class APlayerCharacter;

/**
 *  플레이어의 스태미나 변화를 받아 블루프린트에 넘기는 바.
 *  생김새는 위젯 블루프린트에서 만든다.
 */
UCLASS(abstract)
class ECLIPSE_API UStaminaBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** 게이지를 표시할 플레이어를 정한다. nullptr을 넣으면 연결을 끊는다. */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void BindPlayer(APlayerCharacter* NewTarget);

	UFUNCTION(BlueprintPure, Category = "UI")
	APlayerCharacter* GetTarget() const { return Target.Get(); }


protected:
	virtual void NativeDestruct() override;

	/** 바 채우기와 연출은 블루프린트가 한다. */
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnStaminaUpdated(float Ratio, float Current, float Max);

	void HandleStaminaChanged(float Current, float Max);


private:
	void UnbindTarget();

	/** 위젯은 대상을 소유하지 않는다. 파괴되면 스스로 놓는다. */
	TWeakObjectPtr<APlayerCharacter> Target;
};
