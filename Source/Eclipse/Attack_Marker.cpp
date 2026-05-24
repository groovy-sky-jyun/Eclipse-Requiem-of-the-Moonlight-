// Fill out your copyright notice in the Description page of Project Settings.


#include "Attack_Marker.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AAttack_Marker::AAttack_Marker()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	AttackMarkerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AttackMarkerMesh"));
	AttackMarkerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = AttackMarkerMesh;

	InitialLifeSpan = LifeSpanDuration;

}

// Called when the game starts or when spawned
void AAttack_Marker::BeginPlay()
{
	Super::BeginPlay();
	
	float Scale = AttackRangeRadius / 50.f;
	AttackMarkerMesh->SetWorldScale3D(FVector(Scale, Scale, 0.1f));

#if ENABLE_DRAW_DEBUG
	DrawDebugCylinder(GetWorld(), GetActorLocation(), GetActorLocation() + FVector(0.f, 0.f, 5.f), AttackRangeRadius, 32, FColor::Red, false, LifeSpanDuration);
#endif
}

