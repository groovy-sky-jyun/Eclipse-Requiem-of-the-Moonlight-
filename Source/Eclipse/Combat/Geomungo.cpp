// Fill out your copyright notice in the Description page of Project Settings.


#include "Geomungo.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"

AGeomungo::AGeomungo()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	// 보스 캡슐과 겹쳐도 밀어내거나 공격 판정을 가로채지 않도록 끈다.
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
