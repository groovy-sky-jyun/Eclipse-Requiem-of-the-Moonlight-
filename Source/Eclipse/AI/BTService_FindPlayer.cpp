// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_FindPlayer.h"
#include "BaseEnemyAIController.h"
#include "EnemyBase.h"
#include "BossArena.h"
#include "Eclipse.h"
#include "EclipseGameMode.h"
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
	if (!BB) return;

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return;

	AEnemyBase* Enemy = Cast<AEnemyBase>(AIController->GetPawn());
	if (!Enemy) return;

	// 1. 타겟 확보
	APawn* Player = Cast<APawn>(BB->GetValueAsObject(ABaseEnemyAIController::BB_TargetActor));
	if (!Player)
	{
		Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		if (!Player)
		{
			BB->SetValueAsBool(ABaseEnemyAIController::BB_bIsPlayerInRange, false);
			return;
		}

		BB->SetValueAsObject(ABaseEnemyAIController::BB_TargetActor, Player);
	}

	const FVector PlayerLoc = Player->GetActorLocation();

	// 2. (적 - 플레이어) 거리
	BB->SetValueAsFloat(ABaseEnemyAIController::BB_DistanceToTarget,
		FVector::Dist(Enemy->GetActorLocation(), PlayerLoc));

	// 3. 교전 범위 판정.
	//    아레나가 배치돼 있으면 아레나가 단일 진실 공급원이다.
	//    없으면(테스트 레벨 등) 기존 DetectionRange 방식으로 폴백한다.
	const ABossArena* Arena = nullptr;
	if (const AEclipseGameMode* GameMode = AEclipseGameMode::Get(&OwnerComp))
	{
		Arena = GameMode->GetArena();
	}

	bool bInRange = false;

	if (Arena)
	{
		// 보스 패턴(EclipseVeil, LunarBeam)이 참조하는 중심 좌표를 아레나에 맞춘다.
		// 보스를 레벨에서 옮겨도 패턴 좌표가 흔들리지 않는다.
		BB->SetValueAsVector(ABaseEnemyAIController::BB_CenterLocation, Arena->GetArenaCenter());

		bInRange = Arena->IsInsideArena(PlayerLoc);
	}
	else
	{
		const FVector Center = BB->GetValueAsVector(ABaseEnemyAIController::BB_CenterLocation);
		bInRange = (FVector::Dist(Center, PlayerLoc) <= DetectionRange);
	}

	// 4. 상태가 바뀔 때만 기록. 전투 개시는 아레나가 담당한다.
	const bool bWasInRange = BB->GetValueAsBool(ABaseEnemyAIController::BB_bIsPlayerInRange);
	if (bInRange != bWasInRange)
	{
		BB->SetValueAsBool(ABaseEnemyAIController::BB_bIsPlayerInRange, bInRange);

		UE_LOG(LogEclipse, Verbose, TEXT("[FindPlayer] %s : 플레이어 %s"),
			*Enemy->GetName(), bInRange ? TEXT("교전 범위 진입") : TEXT("교전 범위 이탈"));
	}
}
