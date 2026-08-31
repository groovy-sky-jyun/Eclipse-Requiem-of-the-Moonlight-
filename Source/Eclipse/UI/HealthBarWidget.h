// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBarWidget.generated.h"

class ABaseCharacter;

/**
 *  대상의 체력 변화를 받아 블루프린트에 넘기는 체력바.
 *  플레이어와 보스가 같은 클래스를 쓰고, 생김새만 위젯 블루프린트에서 다르게 만든다.
 */
UCLASS(abstract)
class ECLIPSE_API UHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** 체력을 표시할 대상을 정한다. nullptr을 넣으면 연결을 끊는다. */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void BindCharacter(ABaseCharacter* NewTarget);

	UFUNCTION(BlueprintPure, Category = "UI")
	ABaseCharacter* GetTarget() const { return Target.Get(); }


protected:
	virtual void NativeDestruct() override;

	/** 바 채우기와 연출은 블루프린트가 한다. */
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnHealthUpdated(float Ratio, float Current, float Max);

	UFUNCTION()
	void HandleHealthChanged(float Current, float Max);


private:
	void UnbindTarget();

	/** 위젯은 대상을 소유하지 않는다. 파괴되면 스스로 놓는다. */
	TWeakObjectPtr<ABaseCharacter> Target;
};
