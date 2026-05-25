// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBoss.h"
#include "EnemyMinion.h"
#include "BossAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"
#include "Attack_BloodBolt.h"

AEnemyBoss::AEnemyBoss()
{
	PrimaryActorTick.bCanEverTick = true;

	MaxHealth = 1500.f;
	CurrentHealth = MaxHealth;
	CurrentPhase = 1;

}

void AEnemyBoss::BeginPlay()
{
	Super::BeginPlay();

	AI = Cast<ABossAIController>(GetController());
	if (!AI)
	{
		UE_LOG(LogTemp, Error, TEXT("[Boss] AI Controller is NULL"));
		return;
	}

	BB = AI->GetBlackboardComponent();
	if (!BB)
	{
		UE_LOG(LogTemp, Error, TEXT("[AIController] Blackboard is NULL"));
		return;
	}

	
	// 공중에서 시작
	//SetFlying(true);
}


// ── 데미지 / 사망 ─────────────────────────────────────────────
void AEnemyBoss::HandleTakeDamage_Implementation(float DamageAmount, AActor* Attacker)
{
	if (!CanBeDamaged()) return;

	Super::HandleTakeDamage_Implementation(DamageAmount, Attacker);

	if (!AI || !BB) return;

	float Now = GetWorld()->GetTimeSeconds();
	if ((Now - TimeSinceLastHit) > StaggerResetTime) //StaggerResetTime 안에 다음 피격이 들어와야함.
	{
		StaggerAccumulated = 0.f;
	}
	TimeSinceLastHit = Now;

	StaggerAccumulated += DamageAmount;

	if (StaggerAccumulated >= StaggerThreshold)
	{
		// 스태거 발동
		StaggerAccumulated = 0.f;
		BB->SetValueAsBool(ABossAIController::BB_bIsStaggered, true);
	}
}

void AEnemyBoss::Die_Implementation()
{
	Super::Die_Implementation();

	UE_LOG(LogTemp, Warning, TEXT("Boss is Dead. GAME CLEAR"));
}


// ── 페이즈 ─────────────────────────────────────────────
void AEnemyBoss::EnterPhase(int32 NewPhase)
{
	if (CurrentPhase == NewPhase) return;

	CurrentPhase = NewPhase;
	UE_LOG(LogTemp, Warning, TEXT("[BOSS] Enter : Phase %d"), CurrentPhase);

	switch (CurrentPhase)
	{
	case 2:
		FlyHeight = 500.f;
		//SetFlying(true);
		bEclipseVeilUsed = false;
		break;

	case 3:
		FlyHeight = 600.f;
		//SetFlying(true);
		GetCharacterMovement()->MaxFlySpeed = 900.f;
		break;

	default:
		break;
	}
}


// ── 비행 전환 ─────────────────────────────────────────────────
void AEnemyBoss::SetFlying(bool bFly)
{
	bIsFlying = bFly;
	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	if (!MovementComp) return;

	if (bFly)
	{
		MovementComp->SetMovementMode(MOVE_Flying);
		MovementComp->GravityScale = 0.f;
		MovementComp->MaxFlySpeed = (CurrentPhase >= 3) ? 900.f : 700.f;
		MovementComp->BrakingDecelerationFlying = 2000.f;

		FVector Loc = GetActorLocation();
		Loc.Z = FlyHeight;
		SetActorLocation(Loc);
	}
	else
	{
		MovementComp->SetMovementMode(MOVE_Walking);
		MovementComp->GravityScale = 1.f;
	}
}

// ── 공격 실행 진입점 ──────────────────────────────────────────
void AEnemyBoss::ExecuteAttack(EBossAttackType Attack)
{
	AttackLastUsedList.Add(Attack, GetWorld()->GetTimeSeconds());

	switch (Attack)
	{
	case EBossAttackType::BloodBolt:     Attack_BloodBolt();     break;
	case EBossAttackType::ShadowCrash:	 Attack_ShadowCrash();	 break;
	case EBossAttackType::WraithDrop:    Attack_WraithDrop();    break;
	case EBossAttackType::DarkSweep:     Attack_DarkSweep();     break;
	case EBossAttackType::LunarBeam:     Attack_LunarBeam();     break;
	case EBossAttackType::EclipseVeil:   Defense_EclipseVeil();  break;
	default: break;
	}
}

void AEnemyBoss::SetInvincible(bool bInvincible)
{
	SetCanBeDamaged(!bInvincible);
}


// ── 개별 공격 구현 ────────────────────────────────────────────
void AEnemyBoss::Attack_BloodBolt()
{
	if (!BloodBoltClass) return;

	APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!Player) return;

	int32 BoltCount = (CurrentPhase == 1) ? 3 : 5;
	float BoltSpeed = (CurrentPhase == 3) ? 2200.f : 1800.f;

	// 발사 간격
	float FireInterval = 0.35f;

	BloodBoltRemaining = BoltCount;

	// 첫 발은 즉시 발사, 이후 타이머로 연속 발사
	BloodBolt_FireSingleBolt();

	GetWorldTimerManager().SetTimer(
		BloodBoltTimerHandle,
		this,
		&AEnemyBoss::BloodBolt_FireSingleBolt,
		FireInterval,
		true
	);

	UE_LOG(LogTemp, Warning, TEXT("Attack : BloodBolt"));
}

void AEnemyBoss::BloodBolt_FireSingleBolt()
{
	BloodBoltRemaining--;

	if (BloodBoltRemaining <= 0)
	{
		GetWorldTimerManager().ClearTimer(BloodBoltTimerHandle);
	}

	APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!Player || !BloodBoltClass) return;

	FVector SpawnLoc = GetActorLocation() + GetActorForwardVector() * 100.f + FVector(0.f, 0.f, -50.f);
	FVector Direction = (Player->GetActorLocation() - SpawnLoc).GetSafeNormal();
	FRotator SpawnRot = Direction.Rotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AAttack_BloodBolt* Bolt = GetWorld()->SpawnActor<AAttack_BloodBolt>(BloodBoltClass, SpawnLoc, SpawnRot, SpawnParams);

	if (Bolt)
	{
		Bolt->SetDamage(30.f);
		Bolt->Launch(Direction);
	}
}

void AEnemyBoss::Attack_DarkSweep()
{
	APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!Player) return;

	// 돌진 방향
	FVector ToPlayer = (Player->GetActorLocation() - GetActorLocation());
	ToPlayer.Z = 0.f;
	DarkSweepDirection = ToPlayer.GetSafeNormal();

	// 돌진 시작/끝 위치
	FVector Origin = GetActorLocation();
	Origin.Z = Player->GetActorLocation().Z + DarkSweepHeight;

	DarkSweepStartLoc = Origin;
	DarkSweepEndLoc = Origin + DarkSweepDirection * DarkSweepDistance;

	bDarkSweepHit = false;

	UE_LOG(LogTemp, Warning, TEXT("Attack : DarkSweep"));

	DarkSweep_StartTelegraph();
}

// 1. ShadowCrash : Marker 표시
void AEnemyBoss::DarkSweep_StartTelegraph()
{
	if (AttackMarkerClass)
	{
		FVector MarkerCenter = (DarkSweepStartLoc + DarkSweepEndLoc) * 0.5f;
		MarkerCenter.Z = DarkSweepStartLoc.Z;

		FRotator MarkerRot = DarkSweepDirection.Rotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		AAttack_Marker* Marker = GetWorld()->SpawnActor<AAttack_Marker>(AttackMarkerClass, MarkerCenter, MarkerRot, SpawnParams);
		if(Marker)
		{
			Marker->SetRectMarker(DarkSweepDistance, DarkSweepHalfWidth * 2, 0.8);
		}
	}

	GetWorldTimerManager().SetTimer(
		DarkSweepTimer,
		this,
		&AEnemyBoss::DarkSweep_StartDash,
		0.8f,
		false
	);
}

// 1. ShadowCrash : 돌진
void AEnemyBoss::DarkSweep_StartDash()
{
	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		MovementComponent->SetMovementMode(MOVE_Flying);
		MovementComponent->GravityScale = 0.f;
	}

	SetActorLocation(DarkSweepStartLoc);
	SetActorRotation(DarkSweepDirection.Rotation());

	// 거리 / 속도 = 돌진 소요 시간
	float DashDuration = DarkSweepDistance / DarkSweepSpeed;
	const int32 Steps = 12;
	float StepTime = DashDuration / Steps;

	for (int32 i = 1; i < Steps; i++)
	{
		float Alpha = (float)i / Steps;
		FVector StepLoc = FMath::Lerp(DarkSweepStartLoc, DarkSweepEndLoc, Alpha);

		FTimerHandle TempHandle;
		FTimerDelegate Delegate;
		Delegate.BindLambda([this, StepLoc]()
		{
			if (!IsValid(this)) return;

			SetActorLocation(StepLoc);

			DarkSweep_CheckHit(StepLoc);
		});
		GetWorldTimerManager().SetTimer(TempHandle, Delegate, StepTime * i, false);
	}

	GetWorldTimerManager().SetTimer(
		DarkSweepTimer,
		this,
		&AEnemyBoss::DarkSweep_End,
		DashDuration,
		false
	);
}

// 1. ShadowCrash : 충돌 감지
void AEnemyBoss::DarkSweep_CheckHit(const FVector& CurrentStepLoc)
{
	if (bDarkSweepHit) return;

	APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!Player) return;

	FVector ToPlayer = Player->GetActorLocation() - CurrentStepLoc;
	float FwdDot = FVector::DotProduct(ToPlayer, DarkSweepDirection);
	FVector Lateral = ToPlayer - DarkSweepDirection * FwdDot;
	float LatDist = Lateral.Size2D();

	// 전방 범위 내 + 폭 범위 내 
	// #####[**따로 옵시디언에 정리**]
	if (FwdDot >= -200.f && FwdDot <= 200.f && LatDist <= DarkSweepHalfWidth)
	{
		if (Player->Implements<UCombatInterface>())
		{
			ICombatInterface::Execute_HandleTakeDamage(
				Player, DarkSweepDamage, this);
			bDarkSweepHit = true;

			UE_LOG(LogTemp, Warning,TEXT("[DarkSweep] Hit"));

			// 플레이어 넉백 적용
			// Player->ApplyKnockback(DarkSweepDirection, 800.f);
		}
	}
}

void AEnemyBoss::DarkSweep_End()
{
	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		MovementComponent->SetMovementMode(MOVE_Walking);
		MovementComponent->GravityScale = 1.f;
	}
}


void AEnemyBoss::Attack_ShadowCrash()
{
	APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!Player) return;

	ShadowCrashTargetLoc = Player->GetActorLocation();
	ShadowCrashOriginLoc = GetActorLocation();

	UE_LOG(LogTemp, Warning, TEXT("Attack : ShadowCrash"));

	ShadowCrash_StartAscend();
}

// 1. ShadowCrash : 상승
void AEnemyBoss::ShadowCrash_StartAscend()
{	
	FVector RiseTarget = GetActorLocation() + FVector(0.f, 0.f, 300.f);

	const float AscendDuration = 0.8f;
	const int32 Steps = 16;
	float StepTime = AscendDuration / Steps;

	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		MovementComponent->SetMovementMode(MOVE_Flying);
		MovementComponent->GravityScale = 0.f;
	}

	for (int32 i = 1; i <= Steps; i++)
	{
		float Alpha = (float)i / Steps;
		FVector StepLoc = FMath::Lerp(ShadowCrashOriginLoc, RiseTarget, Alpha);

		FTimerHandle TempHandle;
		FTimerDelegate Delegate;
		Delegate.BindLambda([this, StepLoc]()
		{
				if (IsValid(this)) SetActorLocation(StepLoc, false, nullptr, ETeleportType::TeleportPhysics);
		});
		GetWorldTimerManager().SetTimer(TempHandle, Delegate, StepTime * i, false);
	}

	GetWorldTimerManager().SetTimer(
		ShadowCrashTimer,
		this,
		&AEnemyBoss::ShadowCrash_StartTelegraph,
		AscendDuration,
		false
	);
}

// 2. ShadowCrash : Warning Marker
void AEnemyBoss::ShadowCrash_StartTelegraph()
{
	if (AttackMarkerClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		AAttack_Marker* Marker = GetWorld()->SpawnActor<AAttack_Marker>(
			AttackMarkerClass,
			ShadowCrashTargetLoc,
			FRotator::ZeroRotator,
			SpawnParams
		);

		if (Marker)
		{
			Marker->SetCircleMarker(500.f, 2.f);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ShadowCrash Warning Marker is Null"));
	}

	GetWorldTimerManager().SetTimer(
		ShadowCrashTimer,
		this,
		&AEnemyBoss::ShadowCrash_StartDive,
		1.f,
		false
	);
}

// 3. ShadowCrash : 강하
void AEnemyBoss::ShadowCrash_StartDive()
{
	FVector DiveStart = GetActorLocation();
	FVector DiveEnd = ShadowCrashTargetLoc;

	const float DiveDuration = 0.8f;
	const int32 Steps = 10;
	float StepTime = DiveDuration / Steps;

	for (int32 i = 1; i <= Steps; i++)
	{
		float Alpha = (float)i / Steps;
		FVector StepLoc = FMath::Lerp(DiveStart, DiveEnd, Alpha);

		FTimerHandle TempHandle;
		FTimerDelegate Delegate;
		Delegate.BindLambda([this, StepLoc]()
		{
			if (IsValid(this)) SetActorLocation(StepLoc);
		});
		GetWorldTimerManager().SetTimer(TempHandle, Delegate, StepTime * i, false);
	}

	SetFlying(false);


	GetWorldTimerManager().SetTimer(
		ShadowCrashTimer,
		this,
		&AEnemyBoss::ShadowCrash_OnImpact,
		DiveDuration,
		false
	);
}

// 4. ShadowCrash : 충돌 판정
void AEnemyBoss::ShadowCrash_OnImpact()
{
	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		MovementComponent->GravityScale = 1.f;
		MovementComponent->SetMovementMode(MOVE_Walking);
	}
}

// 5. ShadowCrash : 할퀴기 연계 공격 3회

void AEnemyBoss::ShadowCrash_DoClawHit()
{
}


void AEnemyBoss::Attack_WraithDrop()
{
	if (!MinionClass || !AI) return;
	if (ActiveWraithCount > 0) return;

	int32 SpawnCount = (CurrentPhase >= 3) ? 4 : 2;

	
	UWorld* World = GetWorld();
	if (!World) return;

	for (int32 i = 0; i < SpawnCount; i++)
	{
		APawn* Player = UGameplayStatics::GetPlayerPawn(World, 0);
		if (!Player || !MinionClass) return;

		FVector SpawnLoc = GetActorLocation() + GetActorForwardVector() * 100.f + FVector(FMath::RandRange(-80, 80), FMath::RandRange(-80, 80), 0.0f);
		FVector Direction = (Player->GetActorLocation() - SpawnLoc).GetSafeNormal();
		FRotator SpawnRot = Direction.Rotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;// 겹치게 스폰안되도록 변경

		AEnemyMinion* Minion = World->SpawnActor<AEnemyMinion>(MinionClass, SpawnLoc, SpawnRot, SpawnParams);

		if (Minion)
		{
			++ActiveWraithCount;
		}
	}

	AI->GetBlackboardComponent()->SetValueAsInt(ABossAIController::BB_ActiveWraithCount, ActiveWraithCount);

	UE_LOG(LogTemp, Warning, TEXT("[BOSS] Spawn : %d Wraith"), SpawnCount);
}

void AEnemyBoss::Attack_LunarBeam()
{
	UE_LOG(LogTemp, Warning, TEXT("Attack : LunarBeam"));

	LunarBeamImpactLoc.Empty();

	// 보스가 바라보는 방향 기준
	FVector BossXY = FVector(GetActorLocation().X, GetActorLocation().Y, 0.f);
	FRotator BossYawRot(0.f, GetActorRotation().Yaw, 0.f);

	for (const FVector& LocalOffset : GetLunarBeamOffsets())
	{
		// 보스 방향 기준 LunarBeam 상대 위치 적용
		FVector WorldOffset = BossYawRot.RotateVector(LocalOffset);
		FVector ImpactXY = BossXY + WorldOffset;

		FVector TraceStart = FVector(ImpactXY.X, ImpactXY.Y, 1000.f);
		FVector TraceEnd = FVector(ImpactXY.X, ImpactXY.Y, -500.f);

		FHitResult GroundHit;
		FCollisionQueryParams TraceParams;
		TraceParams.AddIgnoredActor(this);

		float GroundZ = 0.f;
		if (GetWorld()->LineTraceSingleByChannel(GroundHit, TraceStart, TraceEnd, ECC_WorldStatic, TraceParams))
		{
			GroundZ = GroundHit.ImpactPoint.Z;
		}

		LunarBeamImpactLoc.Add(FVector(ImpactXY.X, ImpactXY.Y, GroundZ));
	}

	LunarBeam_SpawnMarkers();
}

void AEnemyBoss::LunarBeam_SpawnMarkers()
{
	for (const FVector& ImpactLoc : LunarBeamImpactLoc)
	{
		if (AttackMarkerClass)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			
			AAttack_Marker* Marker = GetWorld()->SpawnActor<AAttack_Marker>(AttackMarkerClass, ImpactLoc, FRotator::ZeroRotator, SpawnParams);

			if (Marker)
			{
				Marker->SetCircleMarker(LunarBeamRadius, 2.f);
			}
		}
	}

	GetWorldTimerManager().SetTimer(
		LunarBeamTimer,
		this,
		&AEnemyBoss::LunarBeam_IntensifyMarkers,
		1.0f,
		false
	);
}

void AEnemyBoss::LunarBeam_IntensifyMarkers()
{
	// 후에 머티리얼 파라미터로 색 전환
	// 현재는 디버그로 진한 색 원 추가 표시 (프로토타입용)
#if ENABLE_DRAW_DEBUG
	for (const FVector& ImpactLoc : LunarBeamImpactLoc)
	{
		DrawDebugCircle(GetWorld(), ImpactLoc + FVector(0, 0, 6),
			LunarBeamRadius * 0.9f, 32, FColor::Red, false, 1.0f, 0, 8.f,
			FVector(1, 0, 0), FVector(0, 1, 0));
	}
#endif

	// 0.5초 후 빔 낙하 + 판정
	GetWorldTimerManager().SetTimer(
		LunarBeamTimer,
		this,
		&AEnemyBoss::LunarBeam_Impact,
		0.5f,
		false
	);
}

void AEnemyBoss::LunarBeam_Impact()
{
	APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!Player) return;

	FVector PlayerLoc = Player->GetActorLocation();
	bool bPlayerHit = false;

	for (const FVector& ImpactLoc : LunarBeamImpactLoc)
	{
		float Dist2D = FVector::Dist2D(PlayerLoc, ImpactLoc);

		if (Dist2D <= LunarBeamRadius)
		{
			bPlayerHit = true;

#if ENABLE_DRAW_DEBUG
			// 명중한 원을 흰색으로 표시
			DrawDebugCircle(GetWorld(), ImpactLoc + FVector(0, 0, 7),
				LunarBeamRadius, 32, FColor::White, false, 0.5f, 0, 10.f,
				FVector(1, 0, 0), FVector(0, 1, 0));
#endif
			break; // 여러 원에 동시 피격되어도 데미지는 1번만 //후에 수정
		}
	}

	if (bPlayerHit && Player->Implements<UCombatInterface>())
	{
		ICombatInterface::Execute_HandleTakeDamage(Player, LunarBeamDamage, this);

		UE_LOG(LogTemp, Warning, TEXT("[LunarBeam] Hit"));

		// 플레이어 경직(Flinch) 적용
		// if (APlayerCharacter* PC = Cast<APlayerCharacter>(Player))
		//     PC->ApplyFlinch();
	}

	/* 빔 낙하 이펙트 위치 로그 (추후 Niagara 연결용)
	for (int32 i = 0; i < LunarBeamImpactLoc.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("[LunarBeam] 빔 %d 낙하 위치: %s"),i + 1, *LunarBeamImpactLocs[i].ToString());
	}*/
}

TArray<FVector> AEnemyBoss::GetLunarBeamOffsets() const
{
	return {
	   FVector(0.f, 0.f, 0.f), // 중앙 (보스 정중앙)
	   FVector(LunarBeamOffset, 0.f, 0.f), // 전방
	   FVector(-LunarBeamOffset, 0.f, 0.f), // 후방
	   FVector(0.f, LunarBeamOffset, 0.f), // 우측
	   FVector(0.f, -LunarBeamOffset, 0.f), // 좌측
	};
}



void AEnemyBoss::Defense_EclipseVeil()
{
	if (!AI) return;

	bEclipseVeilUsed = true;

	// 무적 ON
	SetInvincible(true);

	// BB 갱신
	AI->GetBlackboardComponent()->SetValueAsBool(ABossAIController::BB_bCanReceiveDamage, false);

	// 연출: Level Sequence 또는 AnimMontage로 처리
	// 일정 시간 후 무적 해제
	FTimerHandle VeilTimer;
	GetWorldTimerManager().SetTimer(VeilTimer, [this]()
		{
			SetInvincible(false);
			SetCanBeDamaged(true);
			AI->GetBlackboardComponent()->SetValueAsBool(ABossAIController::BB_bCanReceiveDamage, true);
			UE_LOG(LogTemp, Warning, TEXT("[BOSS Attack] EclipseVeil End"));
		}, 8.f, false);

	UE_LOG(LogTemp, Warning, TEXT("[BOSS Attack] EclipseVeil Start"));
}



// ── 기타 ──────────────────────────────────────────────────────
void AEnemyBoss::OnWraithDied()
{
	if (!AI) return;

	ActiveWraithCount = FMath::Max(0, ActiveWraithCount - 1);

	AI->GetBlackboardComponent()->SetValueAsInt(ABossAIController::BB_ActiveWraithCount, ActiveWraithCount);
}



// ── Stagger ──────────────────────────────────────────────────────
void AEnemyBoss::UpdateStaggerThresholdByPhase()
{
	StaggerThreshold = (CurrentPhase == 1) ? 100.f
					: (CurrentPhase == 2) ? 200.f
					: 300.f;
}

