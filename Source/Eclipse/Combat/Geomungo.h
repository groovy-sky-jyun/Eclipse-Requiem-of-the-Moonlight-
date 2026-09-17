// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Geomungo.generated.h"

class USceneComponent;
class USkeletalMeshComponent;

/**
 * 보스의 무기인 거문고.
 * 손에 들 때는 보스 소켓에 붙고, 공명처럼 여러 대가 필요하면 공격이 따로 스폰한다.
 */
UCLASS()
class ECLIPSE_API AGeomungo : public AActor
{
	GENERATED_BODY()

public:
	AGeomungo();

	UFUNCTION(BlueprintPure, Category = "Geomungo")
	USkeletalMeshComponent* GetMesh() const { return Mesh; }

protected:
	// 소켓에 붙을 때 루트 트랜스폼은 덮어써진다. 크기와 오프셋은 Mesh에서 조절한다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;

	// 현 떨림을 본으로 움직이기 위해 SkeletalMesh를 쓴다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> Mesh;
};
