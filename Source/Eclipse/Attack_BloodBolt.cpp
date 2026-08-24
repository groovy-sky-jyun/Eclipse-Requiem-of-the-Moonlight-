// Fill out your copyright notice in the Description page of Project Settings.


#include "Attack_BloodBolt.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "CombatInterface.h"

// Sets default values
AAttack_BloodBolt::AAttack_BloodBolt()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->InitSphereRadius(20.f);

	// 프로파일 이름("Projectile")은 이 프로젝트 설정에도 엔진 기본값에도 없다.
	// 설정 파일에 기대지 않고 응답을 여기서 직접 정한다.
	// Block이어야 ProjectileMovement의 스윕 이동이 blocking hit을 만들고,
	// 그래야 OnComponentHit이 발생한다. Overlap으로 두면 영원히 안 불린다.
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionSphere->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	CollisionSphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

	// 자기 충돌은 자기가 잇는다. 이 구체를 스폰하는 쪽이 기억할 일이 아니다.
	CollisionSphere->OnComponentHit.AddDynamic(this, &AAttack_BloodBolt::OnHit);

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


void AAttack_BloodBolt::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (bHasHit) return;

	AActor* BossOwner = GetOwner();
	if (!OtherActor || OtherActor == this || OtherActor == BossOwner) return;

	bHasHit = true;

	if (OtherActor->Implements<UCombatInterface>())
	{
		// 팀 판정은 맞는 쪽(ABaseCharacter::HandleTakeDamage)이 한다.
		ICombatInterface::Execute_HandleTakeDamage(OtherActor, Damage, BossOwner);
	}

	Destroy();
}