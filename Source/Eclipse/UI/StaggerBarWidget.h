// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StaggerBarWidget.generated.h"

class AEnemyBoss;

/**
 *  보스의 스태거 게이지 변화를 받아 블루프린트에 넘기는 바.
 *  보스 체력바 아래에 붙는다. 생김새는 위젯 블루프린트에서 만든다.
 */
UCLASS(abstract)
class ECLIPSE_API UStaggerBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** 게이지를 표시할 보스를 정한다. nullptr을 넣으면 연결을 끊는다. */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void BindBoss(AEnemyBoss* NewTarget);

	UFUNCTION(BlueprintPure, Category = "UI")
	AEnemyBoss* GetTarget() const { return Target.Get(); }


protected:
	virtual void NativeDestruct() override;

	/** 바 채우기와 연출은 블루프린트가 한다. */
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnStaggerUpdated(float Ratio, float Current, float Max);

	void HandleStaggerChanged(float Current, float Max);


private:
	void UnbindTarget();

	/** 위젯은 대상을 소유하지 않는다. 파괴되면 스스로 놓는다. */
	TWeakObjectPtr<AEnemyBoss> Target;
};
