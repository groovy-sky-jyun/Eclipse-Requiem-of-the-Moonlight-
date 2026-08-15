// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BossAttack.generated.h"

UENUM(BlueprintType)
enum class EBossAttackType : uint8
{
	None           UMETA(DisplayName = "None"),
	BloodBolt      UMETA(DisplayName = "Blood Bolt"),
	ShadowCrash      UMETA(DisplayName = "Shadow Crash"),
	WraithDrop     UMETA(DisplayName = "Wraith Drop"),
	DarkSweep      UMETA(DisplayName = "Dark Sweep"),
	LunarBeam      UMETA(DisplayName = "Lunar Beam"),
	EclipseVeil    UMETA(DisplayName = "Eclipse Veil"),
};

class ECLIPSE_API BossAttack
{
public:
	BossAttack();
	~BossAttack();
};
