// Fill out your copyright notice in the Description page of Project Settings.


#include "Attack_Marker.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values
AAttack_Marker::AAttack_Marker()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	CircleMarkerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CircleMarkerMesh"));
	CircleMarkerMesh->SetupAttachment(RootComponent);
	CircleMarkerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CircleMarkerMesh->SetVisibility(false);

	RectMarkerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RectMarkerMesh"));
	RectMarkerMesh->SetupAttachment(RootComponent);
	RectMarkerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RectMarkerMesh->SetVisibility(false);
}

// Called when the game starts or when spawned
void AAttack_Marker::BeginPlay()
{
	Super::BeginPlay();
}

void AAttack_Marker::SetCircleMarker(float Radius, float Duration)
{
	CircleMarkerMesh->SetVisibility(true);
	RectMarkerMesh->SetVisibility(false);

	float Scale = Radius / 5.f; //50은 후에 원형 메시 반경 기준으로 수정
	CircleMarkerMesh->SetWorldScale3D(FVector(Scale, Scale, 0.05f));

	SetLifeSpan(Duration);

#if ENABLE_DRAW_DEBUG
	DrawDebugCylinder(GetWorld(), GetActorLocation(), GetActorLocation(), Scale, 32, FColor::Red, false, Duration);
#endif
}


void AAttack_Marker::SetRectMarker(float Length, float Width, float Duration)
{
	CircleMarkerMesh->SetVisibility(false);
	RectMarkerMesh->SetVisibility(true);

	float ScaleX = Length / 100.f; //100은 기본 큐브 메시 (100x100x100) 기준으로 후에 조절
	float ScaleY = Width / 100.f;
	RectMarkerMesh->SetWorldScale3D(FVector(ScaleX, ScaleY, 0.05f));

	SetLifeSpan(Duration);

#if ENABLE_DRAW_DEBUG
	FVector BoxCenter = GetActorLocation();
	FQuat BoxRotation = GetActorRotation().Quaternion();
	FVector BoxExtent = FVector(50.f * ScaleX, 50.f * ScaleY, 50.f * 0.05f);

	// 보스의 진행 방향(회전)에 맞춰 직사각형 디버그 그리기
	DrawDebugBox(
		GetWorld(),
		BoxCenter,
		BoxExtent,
		BoxRotation,
		FColor::Red,
		false,
		Duration,
		0,
		2.f
	);
#endif
}

