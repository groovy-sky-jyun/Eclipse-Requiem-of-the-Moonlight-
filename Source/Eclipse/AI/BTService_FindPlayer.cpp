// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_FindPlayer.h"
#include "BossAIController.h"
#include "EnemyBoss.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"



UBTService_FindPlayer::UBTService_FindPlayer()
{
	NodeName = TEXT("Find Player");
	
	// Tick Node Time Cycle
	Interval = 0.1f;
	RandomDeviation = 0.05f;
}

void UBTService_FindPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AEnemyBoss* Boss = Cast<AEnemyBoss>(OwnerComp.GetAIOwner()->GetPawn());
	if (!BB || !Boss) return;

	UObject* TargetObject = BB->GetValueAsObject(ABossAIController::BB_TargetActor);
	APawn* Player = Cast<APawn>(TargetObject);
	if (!Player)
	{
		BB->SetValueAsBool(ABossAIController::BB_bIsPlayerInRange, false);
		Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		if (Player)
		{
			BB->SetValueAsObject(ABossAIController::BB_TargetActor, Player);
		}
		else
		{
			return;
		}
	}

	// (보스 - 플레이어) 거리 설정
	float Dist = FVector::Dist(Boss->GetActorLocation(), Player->GetActorLocation());
	BB->SetValueAsFloat(ABossAIController::BB_DistanceToTarget, Dist);

	// 경기장 내에 플레이어 감지 여부
	FVector Center = BB->GetValueAsVector(ABossAIController::BB_BossInitLocation);
	float DetectDist = FVector::Dist(Center, Player->GetActorLocation());

	bool bCurrentPlayerInRange = BB->GetValueAsBool(ABossAIController::BB_bIsPlayerInRange);
	bool bCurrentInCombat = BB->GetValueAsBool(ABossAIController::BB_bIsInCombat);

	if (!bCurrentPlayerInRange && DetectDist <= DetectionRange)
	{
		BB->SetValueAsBool(ABossAIController::BB_bIsPlayerInRange, true);

		if (!bCurrentInCombat)
		{
			BB->SetValueAsBool(ABossAIController::BB_bIsInCombat, true);
			UE_LOG(LogTemp, Warning, TEXT("[FindPlayer] Boss Start Combat"));
		}
			
		UE_LOG(LogTemp, Warning, TEXT("[FindPlayer] Player In Range"));
	}
	else if (bCurrentPlayerInRange && DetectDist > DetectionRange)
	{
		BB->SetValueAsBool(ABossAIController::BB_bIsPlayerInRange, false);

		UE_LOG(LogTemp, Warning, TEXT("[FindPlayer] Player Out Range"));
	}

#if ENABLE_DRAW_DEBUG
	// 감지 범위 — 초록색
	DrawDebugCircle(
		GetWorld(),
		Center,          // 중심 위치
		DetectionRange,          // 반경
		64,                   // 세그먼트 수
		FColor::Green,
		false,                // 지속 여부 (false = 매 프레임 갱신)
		0.2f,                 // 유지 시간 (-1 = 다음 프레임까지)
		0,
		5.f,                  // 선 두께
		FVector(1.f, 0.f, 0.f),  
		FVector(0.f, 1.f, 0.f),
		false
	);
#endif
}