// Fill out your copyright notice in the Description page of Project Settings.


#include "Blade.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Interface/CombatInterface.h"

// Sets default values
ABlade::ABlade()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 1. 충돌 박스(Root) 설정
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);

	// 콜리전 프로필 설정 (Overlap 방식을 사용하여 물리적 튕김 방지)
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 폰(캐릭터)과 겹침 허용

	// 2. 검 메쉬 설정
	BladeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	BladeMesh->SetupAttachment(CollisionBox);
	BladeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 메쉬 자체는 충돌 무시
}

// Called when the game starts or when spawned
void ABlade::BeginPlay()
{
	Super::BeginPlay();
	
	if (CollisionBox)
	{
		CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ABlade::OnOverlapBegin);
	}
}

// Called every frame
void ABlade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABlade::OnOverlapBegin(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor,
	class UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	// 1. 유효성 검사 (자기 자신을 때리거나 null인 경우 무시)
	if (OtherActor && OtherActor != this && OtherActor != GetOwner())
	{
		// 2. 상대방이 ICombatInterface를 가지고 있는지 확인
		if (OtherActor->Implements<UCombatInterface>())
		{
			// 3. 상대방의 진영(TeamTag) 확인 (선택 사항: 아군 오폭 방지)
			FGameplayTag TargetTag = ICombatInterface::Execute_GetTeamTag(OtherActor);
			if (TargetTag == TeamTag) 
			{
				return;
			}

			ICombatInterface::Execute_HandleTakeDamage(OtherActor, BladeDamage, GetOwner());

			// 원래 위치로 되돌아 가는 로직 
		}
	}
}

