// Fill out your copyright notice in the Description page of Project Settings.


#include "Blade.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Interface/CombatInterface.h"
#include "PlayerCharacter.h"

// Sets default values
ABlade::ABlade()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootComponent);

	// 1. 충돌 박스(Root) 설정
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(RootComponent);

	// 콜리전 프로필 설정 (Overlap 방식을 사용하여 물리적 튕김 방지)
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 폰(캐릭터)과 겹침 허용

	// 2. 검 메쉬 설정
	BladeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	BladeMesh->SetupAttachment(RootComponent);
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

	OwnerCharacter = Cast<APlayerCharacter>(GetOwner());

	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("Blade: Owner is null"));
	}
}

// Called every frame
void ABlade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!OwnerCharacter) return;

	switch (CurrentState)
	{
	case EBladeState::Idle:
		// 기존의 부드러운 추격 + 둥둥 로직
		UpdateIdleState(DeltaTime);
		break;

	case EBladeState::Attacking:
		if (TargetAttackLocation.Equals(FVector::ZeroVector)) return;

		// 목표 지점으로 직선 이동 (VInterpTo를 빠르게 쓰거나 직선 이동)
		SetActorLocation(FMath::VInterpTo(GetActorLocation(), TargetAttackLocation, DeltaTime, 20.f));

		// 목표에 거의 도달했으면 복귀 상태로 변경
		if (GetActorLocation().Equals(TargetAttackLocation, 50.f))
		{
			CurrentState = EBladeState::Returning;
		}

		TargetAttackLocation = FVector::ZeroVector;
		break;

	case EBladeState::Returning:
		// 다시 플레이어의 오프셋 위치로 복귀
		FVector ReturnLoc = OwnerCharacter->GetActorLocation() + OwnerCharacter->GetActorRotation().RotateVector(AttachOffset);
		SetActorLocation(FMath::VInterpTo(GetActorLocation(), ReturnLoc, DeltaTime, FollowLocSpeed));

		if (GetActorLocation().Equals(ReturnLoc, 20.f))
		{
			CurrentState = EBladeState::Idle;
		}
		break;
	}
}

void ABlade::OnOverlapBegin(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor,
	class UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OwnerCharacter) return;

	// 1. 유효성 검사 (자기 자신을 때리거나 null인 경우 무시)
	if (OtherActor && OtherActor != this && OtherActor != OwnerCharacter)
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

			ICombatInterface::Execute_HandleTakeDamage(OtherActor, BladeDamage, OwnerCharacter);
		}
	}
}

void ABlade::UpdateIdleState(float DeltaTime)
{
	// 2. 현재 위치에서 목표 위치로 부드럽게 이동 (VInterpTo)
	FVector TargetLocation = OwnerCharacter->GetActorLocation() + OwnerCharacter->GetActorRotation().RotateVector(AttachOffset);
	FVector NextLocation = FMath::VInterpTo(GetActorLocation(), TargetLocation, DeltaTime, FollowLocSpeed);
	SetActorLocation(NextLocation);

	// 4. 둥둥 떠다니는 효과 추가 (Sin 함수 이용)
	RunningTime += DeltaTime;
	float DeltaZ = (FMath::Sin(RunningTime * FloatSpeed) * FloatAmplitude);

	// 5. (0,0,0) 상대위치 기준으로 Mesh의 z 위치만 바꿔줌
	BladeMesh->SetRelativeLocation(FVector(0.f, 0.f, DeltaZ));

	// 검이 플레이어가 바라보는 방향을 같이 바라보게 함 
	FRotator CurrentRot = GetActorRotation();
	FRotator TargetRot = OwnerCharacter->GetActorRotation();

	FRotator NextRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, FollowRotSpeed);
	SetActorRotation(NextRot);
}

void ABlade::Launch(const FVector& TargetLoc)
{
	TargetAttackLocation = TargetLoc;
	CurrentState = EBladeState::Attacking;
}

