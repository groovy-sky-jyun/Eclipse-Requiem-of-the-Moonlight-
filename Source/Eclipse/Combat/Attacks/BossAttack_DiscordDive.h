// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BossAttackBase.h"
#include "BossAttack_DiscordDive.generated.h"

/**
 * 박쥐 떼에 실려 상공으로 오른 뒤, 틀린 음 하나를 신호로 급강하한다.
 */
UCLASS(DisplayName = "Discord Dive")
class ECLIPSE_API UBossAttack_DiscordDive : public UBossAttackBase
{
	GENERATED_BODY()

public:
	UBossAttack_DiscordDive();

protected:
	virtual void OnStartup() override;

	virtual void OnActive() override;

	virtual void OnTick(float DeltaTime) override;

	virtual void OnRecovery() override;

	virtual void OnCancel() override;

	virtual void OnFinish() override;

	/** 보스를 공중으로 띄운다. */
	void Ascend();

	/** 낙하 지점을 확정하고 내리꽂는다. */
	void Dive();

	/** 보스가 실제로 설 자리. 판정 중심에서 캡슐이 겹치지 않을 만큼 물러난 지점이다. */
	FVector GetLandingLocation(const APawn* Player) const;

	/** 착지 지점 원형 판정. 내부와 외곽 링의 피해가 다르다. */
	void ApplyLandingDamage();

	/** 보스를 두 지점 사이로 나눠 옮긴다. 순간이동으로 보이지 않게 한다. */
	void MoveBoss(const FVector& From, const FVector& To, float Duration);

	/** 비행 상태를 되돌린다. 공중에서 불리면 그대로 추락한다. */
	void RestoreMovement();

	/** 낙하 예고 원. 보스가 내려온 만큼 커지고 착지 직전에 걷힌다. */
	void DrawDiveTelegraph();

	/** 지면에 채워진 원판을 그린다. HeightOffset으로 겹치는 순서를 정한다. */
	void DrawTelegraphDisc(float Radius, const FColor& Color, float HeightOffset);

protected:
	/** 박쥐 떼가 날아와 보스에게 붙는 시간. 상승 수단이라 뜨기 전에 온다. */
	UPROPERTY(EditAnywhere, Category = "Settings|Combat|DiscordDive")
	float BatGatherTime = 0.6f;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|DiscordDive")
	float AscendHeight = 900.f;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|DiscordDive")
	float AscendTime = 0.7f;

	/** 상공에 머무는 구간. 보스가 화면 밖이라 플레이어는 소리로만 판단한다. */
	UPROPERTY(EditAnywhere, Category = "Settings|Combat|DiscordDive")
	float HoverTime = 1.2f;

	/** 낙하 지점 확정부터 착지까지. 플레이어의 반응 시간이다. */
	UPROPERTY(EditAnywhere, Category = "Settings|Combat|DiscordDive")
	float DiveTime = 0.4f;

	/** 이동을 나누는 횟수. 많을수록 부드럽다. */
	UPROPERTY(EditAnywhere, Category = "Settings|Combat|DiscordDive", meta = (ClampMin = "1"))
	int32 MoveSteps = 12;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|DiscordDive")
	float InnerRadius = 190.f;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|DiscordDive")
	float OuterRadius = 280.f;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|DiscordDive")
	float InnerDamage = 130.f;

	UPROPERTY(EditAnywhere, Category = "Settings|Combat|DiscordDive")
	float OuterDamage = 80.f;

	/** 캡슐 반지름 합에 더할 여유. 착지 순간 플레이어와 닿지 않게 한다. */
	UPROPERTY(EditAnywhere, Category = "Settings|Combat|DiscordDive")
	float LandingClearance = 20.f;

	/** 예고 원을 지면에서 살짝 띄우는 높이. 바닥과 겹쳐 깜빡이는 것을 막는다. */
	UPROPERTY(EditAnywhere, Category = "Settings|Combat|DiscordDive")
	float TelegraphGroundOffset = 3.f;

	/** 예고 원이 걷히는 낙하 진행률. 이 뒤로는 보스 본체가 착지 지점을 알린다. */
	UPROPERTY(EditAnywhere, Category = "Settings|Combat|DiscordDive", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float TelegraphFadeRatio = 0.85f;

	/** 떠오르기 전 위치. 착지 높이이자 플레이어를 놓쳤을 때의 낙하 지점이다. */
	FVector AscendOrigin = FVector::ZeroVector;

	FVector DiveTargetLocation = FVector::ZeroVector;

	/** 예고 원을 깔 지면 좌표. 낙하 지점을 캡슐 발밑까지 내린 값이다. */
	FVector TelegraphLocation = FVector::ZeroVector;

	/** 낙하 중에만 예고 원을 그린다. */
	bool bDiving = false;

	float DiveElapsedTime = 0.f;

	FTimerHandle DiscordDiveTimer;
};
