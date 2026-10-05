// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "PlayerAttackTransitionTable.generated.h"

/** 연계 공격 트리의 한 노드에 해당된다. 부모 태그, 입력 태그, 입력에 따른 자식 태그  */
USTRUCT(BlueprintType)
struct FPlayerAttackTransition
{
	GENERATED_BODY()

	// 공격의 첫 타는 부모태그를 비워둔다.
	UPROPERTY(EditAnywhere, meta = (Categories = "Attack"))
	FGameplayTag ParentStep;

	UPROPERTY(EditAnywhere, meta = (Categories = "Input"))
	FGameplayTag Input;

	UPROPERTY(EditAnywhere, meta = (Categories = "Attack"))
	FGameplayTag ChildStep;
};

/**
 * 플레이어 공격 연계 트리
 * 저장은 평평한 배열, 실행은 Build로 만든 트리를 쓴다.
 */
USTRUCT(BlueprintType)
struct ECLIPSE_API FPlayerAttackTransitionTable
{
	GENERATED_BODY()

public:
	/** 배열을 읽어 트리를 만든다. 중복과 빈 값은 경고 후 건너뛴다. */
	void Build();

	/** 현재 타에서 이어지는 연계가 없으면 시작 기준으로 찾는다. 없으면 빈 태그 */
	FGameplayTag FindNextStep(const FGameplayTag& CurrentStep, const FGameplayTag& Input) const;

	/** 해당 타에서 이어지는 다음 타 */
	const TMap<FGameplayTag, FGameplayTag>* FindChildren(const FGameplayTag& Step) const;

public:
	/** Details 에서 수정하는 데이터 */
	UPROPERTY(EditAnywhere, meta = (TitleProperty = "{ParentStep} + {Input} -> {ChildStep}"))
	TArray<FPlayerAttackTransition> Transitions;

private:
	// 부모 타 -> (입력 -> 자식 타)
	TMap<FGameplayTag, TMap<FGameplayTag, FGameplayTag>> Tree;
};
