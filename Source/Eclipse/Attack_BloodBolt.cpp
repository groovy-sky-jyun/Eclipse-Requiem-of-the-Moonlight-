// Fill out your copyright notice in the Description page of Project Settings.


#include "Attack_BloodBolt.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "CombatInterface.h"
#include "EnemyMinion.h"

// Sets default values
AAttack_BloodBolt::AAttack_BloodBolt()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->InitSphereRadius(20.f);
	CollisionSphere->SetCollisionProfileName(TEXT("Projectile"));
	RootComponent = CollisionSphere;

	BoltMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoltMesh"));
	BoltMesh->SetupAttachment(RootComponent);
	BoltMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = Speed;
	ProjectileMovement->MaxSpeed = Speed;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	InitialLifeSpan = LifeSpanDuration;
}

// Called when the game starts or when spawned
void AAttack_BloodBolt::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAttack_BloodBolt::Launch(const FVector& Direction)
{
	if (!ProjectileMovement) return;

	ProjectileMovement->Velocity = Direction.GetSafeNormal() * Speed;
}

void AAttack_BloodBolt::SetSpeed(float InSpeed)
{
	Speed = InSpeed;
	if (ProjectileMovement)
	{
		ProjectileMovement->InitialSpeed = InSpeed;
		ProjectileMovement->MaxSpeed = InSpeed;
	}
}



void AAttack_BloodBolt::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	AActor* BossOwner = GetOwner();
	if (!OtherActor || OtherActor == this || OtherActor == BossOwner) return;

	if (OtherActor->Implements<UCombatInterface>())
	{
		ICombatInterface::Execute_HandleTakeDamage(OtherActor, Damage, BossOwner);
	}

	Destroy();
}