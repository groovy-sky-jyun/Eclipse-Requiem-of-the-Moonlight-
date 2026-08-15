// Fill out your copyright notice in the Description page of Project Settings.


#include "Blade.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "CombatInterface.h"
#include "PlayerCharacter.h"
#include "Attack_BloodBolt.h"

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
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 폰(캐릭터)과 겹침 허용

	// 2. 검 메쉬 설정
	BladeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	BladeMesh->SetupAttachment(RootComponent);
	BladeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 메쉬 자체는 충돌 무시

	SetCollisionActive(false);
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

	if (!IsValid(OwnerCharacter))
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
	{
		if (TargetAttackLocation.Equals(FVector::ZeroVector)) return;

		TargetAttackLocation.Z = GetActorLocation().Z;
		// 목표 지점으로 직선 이동 (VInterpTo를 빠르게 쓰거나 직선 이동)
		SetActorLocation(FMath::VInterpTo(GetActorLocation(), TargetAttackLocation, DeltaTime, 20.f));

		// 목표에 거의 도달했으면 복귀 상태로 변경
		if (GetActorLocation().Equals(TargetAttackLocation, 50.f))
		{
			SetCollisionActive(false);
			HitActorsThisSwing.Empty();

			CurrentState = EBladeState::Returning;
			TargetAttackLocation = FVector::ZeroVector;
		}
		break;
	}
	case EBladeState::Returning:
	{
		// 다시 플레이어의 오프셋 위치로 복귀
		const FVector ReturnLoc = OwnerCharacter->GetActorLocation() + OwnerCharacter->GetActorRotation().RotateVector(AttachOffset);
		SetActorLocation(FMath::VInterpTo(GetActorLocation(), ReturnLoc, DeltaTime, FollowLocSpeed));

		if (GetActorLocation().Equals(ReturnLoc, 20.f))
		{
			CurrentState = EBladeState::Idle;
		}
		break;
	}
	}
}


// ── 충돌 판정 ─────────────────────────────────────────────────
void ABlade::OnOverlapBegin(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor,
	class UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (CurrentState != EBladeState::Attacking) return;
	if (!OwnerCharacter) return;
	if (!IsValid(OtherActor) || OtherActor == this || OtherActor == OwnerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("Overlap OtherActor is null"));
		return;
	}

	if (HitActorsThisSwing.Contains(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("Overlap OtherActor alreay Contains HitActor List"));
		return;
	}

	if (!OtherActor->Implements<UCombatInterface>()) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Overlap OtherActor have not CombatInterface"));
		return;
	}

	FGameplayTag TargetTag = ICombatInterface::Execute_GetTeamTag(OtherActor);
	if (TargetTag == OwnerCharacter->GetTeamTag_Implementation())
	{
		UE_LOG(LogTemp, Warning, TEXT("Team Tag is Same To Player"));
		return;
	}

	// ── 데미지 전달 ──────────────────────────────────────────
	HitActorsThisSwing.Add(OtherActor); // 중복 방지 등록
	BladeDamage = OwnerCharacter->GetComboDamage(OwnerCharacter->GetComboIndex());
	ICombatInterface::Execute_HandleTakeDamage(OtherActor, BladeDamage, OwnerCharacter);

	OwnerCharacter->UpdateBasicCombo();
}

void ABlade::UpdateIdleState(float DeltaTime)
{
	const FVector OwnerOffset = OwnerCharacter->GetActorRotation().RotateVector(AttachOffset);
	const FVector TargetLocation = OwnerCharacter->GetActorLocation() + OwnerOffset;

	// 2. 현재 위치에서 목표 위치로 부드럽게 이동 (VInterpTo)
	SetActorLocation(FMath::VInterpTo(GetActorLocation(), TargetLocation, DeltaTime, FollowLocSpeed));

	// 4. 둥둥 떠다니는 효과 추가 (Sin 함수 이용)
	RunningTime += DeltaTime;
	const float DeltaZ = (FMath::Sin(RunningTime * FloatSpeed) * FloatAmplitude);

	// 5. (0,0,0) 상대위치 기준으로 Mesh의 z 위치만 바꿔줌
	BladeMesh->SetRelativeLocation(FVector(0.f, 0.f, DeltaZ));

	// 검이 플레이어가 바라보는 방향을 같이 바라보게 함 
	const FRotator NextRot = FMath::RInterpTo(GetActorRotation(), OwnerCharacter->GetActorRotation(), DeltaTime, FollowRotSpeed);
	SetActorRotation(NextRot);
}

void ABlade::Launch(const FVector& TargetLoc)
{
	TargetAttackLocation = TargetLoc;
	HitActorsThisSwing.Empty();
	SetCollisionActive(true);
	CurrentState = EBladeState::Attacking;
}

void ABlade::SetCollisionActive(bool bActive)
{
	if (!CollisionBox) return;
	CollisionBox->SetCollisionEnabled(bActive ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}
