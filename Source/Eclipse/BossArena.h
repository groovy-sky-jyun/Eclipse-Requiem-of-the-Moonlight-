// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossArena.generated.h"

class APlayerCharacter;
class AEclipseGameMode;

/**
*  보스전이 벌어지는 공간을 정의
*
*  - 아레나 중심 / 보스 패턴(EclipseVeil, LunarBeam)의 좌표 기준
*  - 플레이어 진입 감지 → 전투 시작
*  - 전투 중 플레이어의 이탈 차단
*
*  액터 위치가 곧 아레나 중심이다.
*/

UCLASS()
class ECLIPSE_API ABossArena : public AActor
{
	GENERATED_BODY()

public:
	ABossArena();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;


// ── 조회 ──────────────────────────────────────────────────
public:
	/** 아레나 중심 위치 그대로 사용 */
	UFUNCTION(BlueprintPure, Category = "Arena")
	FVector GetArenaCenter() const { return GetActorLocation(); }

	UFUNCTION(BlueprintPure, Category = "Arena")
	float GetArenaRadius() const { return ArenaRadius; }

	/** 전투 시작 트리거 반경. 0 이하로 두면 ArenaRadius를 그대로 사용 */
	UFUNCTION(BlueprintPure, Category = "Arena")
	float GetEngageRadius() const { return (EngageRadius > 0.f) ? EngageRadius : ArenaRadius; }

	/** 해당 위치가 아레나 안인가. XY 평면 기준(높이 무시). */
	UFUNCTION(BlueprintPure, Category = "Arena")
	bool IsInsideArena(const FVector& Location) const;


// ── 설정 ──────────────────────────────────────────────────
protected:
	/** 아레나 반경 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Arena", meta = (ClampMin = "100.0"))
	float ArenaRadius = 1500.f;

	/** 전투를 시작시키는 진입 반경	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Arena", meta = (ClampMin = "0.0"))
	float EngageRadius = 1200.f;

	/** 전투 중 플레이어의 이탈을 막을지 여부. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Arena")
	bool bConfinePlayer = true;

	/** 아레나 경계를 디버그 원으로 표시 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Debug")
	bool bShowDebug = true;


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;


private:
	/** 플레이어를 경계 안에 가둔다. */
	void ConfinePlayer(APlayerCharacter* Player) const;

	APlayerCharacter* GetPlayer();

	/**
	 * 플레이어는 아레나 BeginPlay 시점에 아직 스폰되지 않았을 수 있고,
	 * 도중에 파괴/교체될 수도 있다. 아레나는 플레이어의 관찰자일 뿐 소유자가 아니므로
	 * 약한 참조로 들고 무효해질 때마다 다시 찾는다.
	 */
	TWeakObjectPtr<APlayerCharacter> CachedPlayer;

	/** GameMode는 레벨당 하나이며 BeginPlay 시점에 이미 존재한다. 한 번만 캐싱한다. */
	UPROPERTY()
	TObjectPtr<AEclipseGameMode> GameMode;

	/** 폰 타입 불일치 경고를 매 프레임 쏟지 않기 위한 플래그 */
	bool bWarnedPlayerClass = false;
};
