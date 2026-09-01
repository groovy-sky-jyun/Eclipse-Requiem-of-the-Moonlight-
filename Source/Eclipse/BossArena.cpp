// Fill out your copyright notice in the Description page of Project Settings.


#include "BossArena.h"
#include "Eclipse.h"
#include "EclipseGameMode.h"
#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

ABossArena::ABossArena()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
}

void ABossArena::BeginPlay()
{
	Super::BeginPlay();

	// GameMode는 레벨당 하나이고 액터 BeginPlay 시점에 이미 존재하므로 여기서 한 번만 캐싱한다.
	GameMode = AEclipseGameMode::Get(this);
	if (!GameMode)
	{
		UE_LOG(LogEclipse, Error, TEXT("[Arena] GameMode not found. Arena disabled"));
		SetActorTickEnabled(false);
		return;
	}

	// 보스 패턴과 BT가 이 아레나를 참조하게 된다.
	GameMode->RegisterArena(this);
}

void ABossArena::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!GameMode)
	{
		SetActorTickEnabled(false);
		return;
	}

	// 승패가 확정되면 아레나가 할 일이 없다. 구속이 풀리고 틱도 멈춘다.
	if (GameMode->GetBattleResult() == EBattleResult::Victory ||
		GameMode->GetBattleResult() == EBattleResult::Defeat)
	{
		SetActorTickEnabled(false);
		return;
	}

#if ENABLE_DRAW_DEBUG
	if (bShowDebug)
	{
		const FVector DebugCenter = GetArenaCenter() + FVector(0.f, 0.f, 10.f);
		const FVector AxisX(1.f, 0.f, 0.f);
		const FVector AxisY(0.f, 1.f, 0.f);

		// 이탈 차단 경계 — 빨강
		DrawDebugCircle(GetWorld(), DebugCenter, ArenaRadius, 64,
			FColor::Red, false, -1.f, 0, 8.f, AxisX, AxisY, false);

		// 전투 개시 트리거 — 초록
		DrawDebugCircle(GetWorld(), DebugCenter, GetEngageRadius(), 64,
			FColor::Green, false, -1.f, 0, 5.f, AxisX, AxisY, false);
	}
#endif

	APlayerCharacter* Player = GetPlayer();
	if (!Player) return;

	if (GameMode->IsBattleActive())
	{
		// 전투 중 —> 이탈 차단
		if (bConfinePlayer)
		{
			ConfinePlayer(Player);
		}
	}
	else
	{
		// 전투 전 —> 진입 감지 (이탈 차단 하지 않은 상태)
		if (FVector::Dist2D(Player->GetActorLocation(), GetArenaCenter()) <= GetEngageRadius())
		{
			UE_LOG(LogEclipse, Log, TEXT("[Arena] Player entered. Battle start"));
			GameMode->StartBattle();
		}
	}
}


// ── 조회 ──────────────────────────────────────────────────────────────
bool ABossArena::IsInsideArena(const FVector& Location) const
{
	return FVector::Dist2D(Location, GetArenaCenter()) <= ArenaRadius;
}


// ── 이탈 차단 ──────────────────────────────────────────────────────────
void ABossArena::ConfinePlayer(APlayerCharacter* Player) const
{
	const FVector Center = GetArenaCenter();
	const FVector PlayerLoc = Player->GetActorLocation();

	FVector Offset = PlayerLoc - Center;
	Offset.Z = 0.f;

	const float Dist = Offset.Size();
	if (Dist <= ArenaRadius) return;

	// 벡터 / 크기 = 단위 벡터(방향)
	const FVector OutwardDir = Offset / Dist;

	// 1. 경계면 위로 위치 보정. 높이는 건드리지 않는다(점프/낙하 유지).
	FVector Clamped = Center + OutwardDir * ArenaRadius;
	Clamped.Z = PlayerLoc.Z;
	Player->SetActorLocation(Clamped, true);

	// 2. 바깥으로 향하는 속도 성분만 제거한다.
	if (UCharacterMovementComponent* Movement = Player->GetCharacterMovement())
	{
		const float OutwardSpeed = FVector::DotProduct(Movement->Velocity, OutwardDir);
		if (OutwardSpeed > 0.f)
		{
			Movement->Velocity -= OutwardDir * OutwardSpeed;
		}
	}
}

APlayerCharacter* ABossArena::GetPlayer()
{
	if (CachedPlayer.IsValid()) return CachedPlayer.Get();

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);

	// 아직 스폰되지 않았을 뿐일 수 있다. 정상 상황이므로 조용히 넘어간다.
	if (!PlayerPawn) return nullptr;

	CachedPlayer = Cast<APlayerCharacter>(PlayerPawn);

	// 폰은 있는데 타입이 다르면 설정 실수다. 조용히 실패하지 않도록 한 번만 알린다.
	if (!CachedPlayer.IsValid() && !bWarnedPlayerClass)
	{
		bWarnedPlayerClass = true;
		UE_LOG(LogEclipse, Warning,
			TEXT("[Arena] Controlled pawn is not APlayerCharacter (actual: %s). ")
			TEXT("Check GameMode Default Pawn Class. Boundary block disabled"),
			*PlayerPawn->GetClass()->GetName());
	}

	return CachedPlayer.Get();
}
