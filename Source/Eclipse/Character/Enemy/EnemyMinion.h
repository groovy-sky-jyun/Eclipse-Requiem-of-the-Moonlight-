// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "EnemyMinion.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API AEnemyMinion : public AEnemyBase
{
	GENERATED_BODY()


public:
	AEnemyMinion();


protected:
	virtual void BeginPlay() override;


public:
	virtual void Die_Implementation() override;
};
