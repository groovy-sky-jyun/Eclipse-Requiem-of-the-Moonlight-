// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ReturnToHome.generated.h"

/**
 * 플레이어가 감지 범위를 벗어났을 때 실행
 * 3초 대기 후 스폰 위치로 귀환
 * hp는 초기화 x
 */
UCLASS()
class ECLIPSE_API UBTTask_ReturnToHome : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_ReturnToHome();


protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


private:
	float WaitRemaining = 0.f;
	bool bWaiting = true;
	
	UPROPERTY(EditAnywhere, Category = "ReturnToHome")
	float WaitBeforeReturn = 3.f;

	UPROPERTY(EditAnywhere, Category = "ReturnToHome")
	float ReturnSpeed = 800.f;

	UPROPERTY(EditAnywhere, Category = "ReturnToHome")
	float AcceptanceRadius = 150.f;
};
