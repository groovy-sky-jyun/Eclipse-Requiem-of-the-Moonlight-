// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAttack_EclipseVeil.h"
#include "EnemyBoss.h"
#include "SlashBeam.h"
#include "BossAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

void UBossAttack_EclipseVeil::OnStart()
{
	/*
	// 무적 ON
	GetBoss()->SetInvincible(true);

	// BB 갱신
	GetBoss()->BB->SetValueAsBool(ABossAIController::BB_bCanReceiveDamage, false);

	// 연출: Level Sequence 또는 AnimMontage로 처리
	// 일정 시간 후 무적 해제
	FTimerHandle VeilTimer;
	SetAttackTimer(VeilTimer, FTimerDelegate::CreateWeakLambda(this, [this]()
		{
			GetBoss()->SetInvincible(false);
			GetBoss()->SetCanBeDamaged(true);
			GetBoss()->BB->SetValueAsBool(ABossAIController::BB_bCanReceiveDamage, true);
			UE_LOG(LogTemp, Warning, TEXT("[BOSS Attack] EclipseVeil End"));
		}), 8.f, false);
		*/

	UE_LOG(LogTemp, Warning, TEXT("[BOSS Attack] EclipseVeil Start"));

	EclipseCurrentRound = 0;

	EclipseVeil_StartFog();
}

void UBossAttack_EclipseVeil::EclipseVeil_StartFog()
{
	AEnemyBoss* Boss = GetBoss();

	Boss->SetActorHiddenInGame(true);

	Boss->SetCanBeDamaged(false);

	//camera topview blend (blueprint event로 권장)

	SetAttackTimer(
		EclipseVeilTimer,
		FTimerDelegate::CreateWeakLambda(this, [this]() {EclipseVeil_ExecuteRound(1); }),
		1.5f,
		false
	);
}

void UBossAttack_EclipseVeil::EclipseVeil_ExecuteRound(int32 Round)
{
	AEnemyBoss* Boss = GetBoss();

	TArray<FSlashConfig> Configs = GetSlashConfigs(Round);
	FVector ArenaCenter = FVector::ZeroVector;

	ArenaCenter = Boss->BB->GetValueAsVector(ABossAIController::BB_CenterLocation);

	float HintDelay = 0.f;
	float  SlashDelay = 0.6f;

	for (int32 i = 0; i < Configs.Num(); i++)
	{
		const FSlashConfig& Cfg = Configs[i];
		FVector SlashCenter = ArenaCenter + Cfg.offset;
		float Angle = Cfg.AngleDeg;
		float Damage = EclipseDamage;

		FTimerHandle HintTimer;
		SetAttackTimer(
			HintTimer,
			FTimerDelegate::CreateWeakLambda(this, [this, SlashCenter, Angle]()
			{
#if ENABLE_DRAW_DEBUG
				// 눈 위치: 슬래시 방향의 끝 지점에 표시
				FRotator SlashRot(0.f, Angle, 0.f);
				FVector EyeDir = SlashRot.RotateVector(FVector(1, 0, 0));
				FVector EyeLoc = SlashCenter + EyeDir * 3500.f;
				EyeLoc.Z = SlashCenter.Z + 300.f;
				DrawDebugSphere(GetWorld(), EyeLoc, 50.f, 12, FColor::Red, false, 0.7f);
#endif
			}),
			HintDelay + (i * 0.15f),
			false
		);

		FTimerHandle SlashTimer;
		SetAttackTimer(
			SlashTimer,
			FTimerDelegate::CreateWeakLambda(this, [this, SlashCenter, Angle, Damage]()
			{
				EclipseVeil_SpawnSlash(SlashCenter, Angle, Damage);
			}),
			HintDelay + (i * 0.15f) + SlashDelay,
			false
		);
	}

	float RoundDuration = HintDelay + (Configs.Num() - 1) * 0.15f + SlashDelay + 0.5f;

	SetAttackTimer(
		EclipseVeilTimer,
		FTimerDelegate::CreateWeakLambda(this, [this, Round]()
		{
			if (Round < 3) EclipseVeil_ExecuteRound(Round + 1);
			else Finish();
		}),
		RoundDuration,
		false
	);
}

void UBossAttack_EclipseVeil::EclipseVeil_SpawnSlash(FVector Center, float AngleDeg, float Damage)
{
	AEnemyBoss* Boss = GetBoss();

	if (!SlashBeamClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[EclipseVeil] SlashBeamClass is null!!!"));
		return;
	}

	FRotator SlashRot(0.f, AngleDeg, 0.f);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Boss;

	ASlashBeam* Slash = GetWorld()->SpawnActor<ASlashBeam>(SlashBeamClass, Center, SlashRot, SpawnParams);

	if (Slash) Slash->Activate(Damage, Boss);

#if ENABLE_DRAW_DEBUG
	// 슬래시 위치 시각화
	FVector Dir = SlashRot.RotateVector(FVector(1, 0, 0));
	DrawDebugLine(GetWorld(),
		Center - Dir * 4000.f,
		Center + Dir * 4000.f,
		FColor::Red, false, 0.4f, 0, 8.f);
#endif
}

void UBossAttack_EclipseVeil::OnFinish()
{
	UE_LOG(LogTemp, Warning, TEXT("[EclipseVeil] Finish"));

	AEnemyBoss* Boss = GetBoss();
	if (!IsValid(Boss)) return;

	Boss->SetActorHiddenInGame(false);
	Boss->SetCanBeDamaged(true);
}

TArray<UBossAttack_EclipseVeil::FSlashConfig> UBossAttack_EclipseVeil::GetSlashConfigs(int32 Round) const
{
	// 아레나 중심(HomeLocation) 기준 슬래시 배치
	// AngleDeg: 슬래시 방향 각도 (0 = 가로, 90 = 세로)
	// Offset: 중심에서 얼마나 치우쳤는지

	switch (Round)
	{
	case 1:
		// 십자: 가로 1 + 세로 1
		return {
			{ FVector(0, 0, 0), 0.f  },  // 가로 중앙
			{ FVector(0, 0, 0), 90.f },  // 세로 중앙
		};

	case 2:
		// 십자 + 대각 2 + 오프셋 가로 2
		return {
			{ FVector(0, 0, 0), 0.f },  // 가로 중앙
			{ FVector(0, 0, 0), 90.f },  // 세로 중앙
			{ FVector(0, 0, 0), 45.f },  // 대각 /
			{ FVector(0, 0, 0), 135.f },  // 대각 \ (반대) 
			{ FVector(0, 700, 0), 0.f },  // 가로 위
			{ FVector(0, -700, 0), 0.f },  // 가로 아래
		};

	case 3:
	default:
		// 격자: 라운드2 + 오프셋 세로 2 + 오프셋 대각 4
		return {
			{ FVector(0, 0, 0), 0.f },
			{ FVector(0, 0, 0), 90.f },
			{ FVector(0, 0, 0), 45.f },
			{ FVector(0, 0, 0), 135.f },
			{ FVector(0, 700, 0), 0.f },
			{ FVector(0, -700, 0), 0.f },
			{ FVector(700, 0, 0), 90.f },  // 세로 우
			{ FVector(-700, 0, 0), 90.f },  // 세로 좌
			{ FVector(0, 500, 0), 45.f },  // 대각 / 위
			{ FVector(0, -500, 0), 45.f },  // 대각 / 아래
			{ FVector(0, 500, 0), 135.f },  // 대각 \ 위
			{ FVector(0, -500, 0), 135.f },  // 대각 \ 아래
		};
	}
}
