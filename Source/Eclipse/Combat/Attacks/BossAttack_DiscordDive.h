// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BossAttackBase.h"
#include "BossAttack_DiscordDive.generated.h"

/**
 * 박쥐 떼로 모습을 감춘 뒤 틀린 음 하나를 신호로 급강하한다.
 */
UCLASS(DisplayName = "Discord Dive")
class ECLIPSE_API UBossAttack_DiscordDive : public UBossAttackBase
{
	GENERATED_BODY()

protected:
	virtual void OnStart() override;

	virtual void OnTick(float DeltaTime) override;

	virtual void OnCancel() override;

	virtual void OnFinish() override;
};
