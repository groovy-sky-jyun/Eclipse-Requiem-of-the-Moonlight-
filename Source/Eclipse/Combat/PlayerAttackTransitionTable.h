// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "PlayerAttackTransitionTable.generated.h"

/** 콤보 루트의 한 타. 이 입력이 들어오면 이 공격이 나간다. */
USTRUCT(BlueprintType)
struct FPlayerComboStep
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (Categories = "Input"))
	FGameplayTag Input;

	UPROPERTY(EditAnywhere, meta = (Categories = "Attack"))
	FGameplayTag Attack;

#if WITH_EDITORONLY_DATA
	// Details 요약 표시용. 편집 시 자동 갱신
	UPROPERTY(VisibleAnywhere, meta = (EditCondition = "false", EditConditionHides))
	FString InputToAttackText;
#endif
};

/** 콤보 루트 하나. 1타부터 끝까지 순서대로 둔다. */
USTRUCT(BlueprintType)
struct FPlayerComboRoute
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FName RouteName;

	UPROPERTY(EditAnywhere, meta = (TitleProperty = "InputToAttackText"))
	TArray<FPlayerComboStep> Steps;
};

/** 콤보 진행 위치. 루트 번호와 몇 번째 타인지 */
USTRUCT()
struct FPlayerComboPosition
{
	GENERATED_BODY()

	int32 RouteIndex = INDEX_NONE;

	int32 StepIndex = INDEX_NONE;

	bool IsValid() const { return RouteIndex != INDEX_NONE && StepIndex != INDEX_NONE; }
};

/**
 * 플레이어 콤보 테이블 (콤보 스트링)
 * 지금까지의 입력 순서로 다음 타를 찾는다.
 */
USTRUCT(BlueprintType)
struct ECLIPSE_API FPlayerAttackTransitionTable
{
	GENERATED_BODY()

public:
	/** 빈 값과 같은 입력 순서 충돌을 경고한다. */
	void ValidateRoutes() const;

	/** 현재 위치에서 이어지는 타가 없으면 1타부터 찾는다. 없으면 무효 위치 */
	FPlayerComboPosition FindNextStep(const FPlayerComboPosition& Current, const FGameplayTag& Input) const;

	/** 위치에 해당하는 공격 태그. 무효 위치면 빈 태그 */
	FGameplayTag GetAttack(const FPlayerComboPosition& Position) const;

#if WITH_EDITOR
	/** 모든 타의 InputToAttackText를 "Input -> Attack"으로 채운다. */
	void RefreshInputToAttackTexts();
#endif

private:
	/** Current 다음 칸에 Input이 오는 루트를 찾는다. */
	FPlayerComboPosition FindContinuation(const FPlayerComboPosition& Current, const FGameplayTag& Input) const;

	/** 두 루트의 입력 순서가 StepCount 타까지 같은지 */
	bool HasSameInputs(const FPlayerComboRoute& A, const FPlayerComboRoute& B, int32 StepCount) const;

public:
	UPROPERTY(EditAnywhere, meta = (TitleProperty = "RouteName"))
	TArray<FPlayerComboRoute> Routes;
};
