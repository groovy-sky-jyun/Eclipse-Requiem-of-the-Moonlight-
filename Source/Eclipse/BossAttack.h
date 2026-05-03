// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "BossAttack.generated.h"

UENUM(BlueprintType)
enum class EBossAttackType : uint8
{
	None           UMETA(DisplayName = "없음"),
	BloodBolt      UMETA(DisplayName = "블러드 볼트"),
	WraithDrop     UMETA(DisplayName = "망령 소환"),
	LunarBeam      UMETA(DisplayName = "달빛 포격"),
	DamningTether  UMETA(DisplayName = "저주의 속박"),
	MiasmaStep     UMETA(DisplayName = "안개 기습"),
	EclipseVeil    UMETA(DisplayName = "이클립스 베일"),
};

class ECLIPSE_API BossAttack
{
public:
	BossAttack();
	~BossAttack();
};
