// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAttack_LunarBeam.h"
#include "Eclipse.h"
#include "EnemyBoss.h"
#include "Attack_Marker.h"
#include "CombatInterface.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

void UBossAttack_LunarBeam::OnStart()
{
	AEnemyBoss* Boss = GetBoss();

	UE_LOG(LogEclipse, Log, TEXT("Attack : LunarBeam"));

	LunarBeamImpactLoc.Empty();

	// 보스가 바라보는 방향 기준
	FVector BossXY = FVector(Boss->GetActorLocation().X, Boss->GetActorLocation().Y, 0.f);
	FRotator BossYawRot(0.f, Boss->GetActorRotation().Yaw, 0.f);

	for (const FVector& LocalOffset : GetLunarBeamOffsets())
	{
		// 보스 방향 기준 LunarBeam 상대 위치 적용
		FVector WorldOffset = BossYawRot.RotateVector(LocalOffset);
		FVector ImpactXY = BossXY + WorldOffset;

		FVector TraceStart = FVector(ImpactXY.X, ImpactXY.Y, 1000.f);
		FVector TraceEnd = FVector(ImpactXY.X, ImpactXY.Y, -500.f);

		FHitResult GroundHit;
		FCollisionQueryParams TraceParams;
		TraceParams.AddIgnoredActor(Boss);

		float GroundZ = 0.f;
		if (GetWorld()->LineTraceSingleByChannel(GroundHit, TraceStart, TraceEnd, ECC_WorldStatic, TraceParams))
		{
			GroundZ = GroundHit.ImpactPoint.Z;
		}

		LunarBeamImpactLoc.Add(FVector(ImpactXY.X, ImpactXY.Y, GroundZ));
	}

	LunarBeam_SpawnMarkers();
}

void UBossAttack_LunarBeam::LunarBeam_SpawnMarkers()
{
	AEnemyBoss* Boss = GetBoss();

	for (const FVector& ImpactLoc : LunarBeamImpactLoc)
	{
		if (AttackMarkerClass)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = Boss;
			
			AAttack_Marker* Marker = GetWorld()->SpawnActor<AAttack_Marker>(AttackMarkerClass, ImpactLoc, FRotator::ZeroRotator, SpawnParams);

			if (Marker)
			{
				Marker->SetCircleMarker(LunarBeamRadius, 2.f);
			}
		}
	}

	SetAttackTimer(
		LunarBeamTimer,
		FTimerDelegate::CreateUObject(this, &UBossAttack_LunarBeam::LunarBeam_IntensifyMarkers),
		1.0f,
		false
	);
}

void UBossAttack_LunarBeam::LunarBeam_IntensifyMarkers()
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
	SetAttackTimer(
		LunarBeamTimer,
		FTimerDelegate::CreateUObject(this, &UBossAttack_LunarBeam::LunarBeam_Impact),
		0.5f,
		false
	);
}

void UBossAttack_LunarBeam::LunarBeam_Impact()
{
	// 마커 예열이 끝났다. 여기서 낙하 판정이 나간다.
	SetAttackState(EBossAttackState::Active);

	APawn* Player = GetTargetPlayer();
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
		ICombatInterface::Execute_TakeCombatDamage(Player, LunarBeamDamage, GetBoss());

		UE_LOG(LogEclipse, Log, TEXT("[LunarBeam] Hit"));

		// 플레이어 경직(Flinch) 적용
		// if (APlayerCharacter* PC = Cast<APlayerCharacter>(Player))
		//     PC->ApplyFlinch();
	}

	SetAttackState(EBossAttackState::Recovery);
	Finish();

	/* 빔 낙하 이펙트 위치 로그 (추후 Niagara 연결용)
	for (int32 i = 0; i < LunarBeamImpactLoc.Num(); i++)
	{
		UE_LOG(LogEclipse, Verbose, TEXT("[LunarBeam] Beam %d impact at %s"),i + 1, *LunarBeamImpactLocs[i].ToString());
	}*/
}

TArray<FVector> UBossAttack_LunarBeam::GetLunarBeamOffsets() const
{
	return {
	   FVector(0.f, 0.f, 0.f), // 중앙 (보스 정중앙)
	   FVector(LunarBeamOffset, 0.f, 0.f), // 전방
	   FVector(-LunarBeamOffset, 0.f, 0.f), // 후방
	   FVector(0.f, LunarBeamOffset, 0.f), // 우측
	   FVector(0.f, -LunarBeamOffset, 0.f), // 좌측
	};
}
