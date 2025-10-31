// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_FindPatrolPos.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ABAI.h"
#include "Interface/ABCharacterAIInterface.h"

UBTTask_FindPatrolPos::UBTTask_FindPatrolPos()
{
}

EBTNodeResult::Type UBTTask_FindPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	// AIController가 소유 중인 폰 정보 가져오기.
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}

	// 랜덤 위치를 선택해서 블랙보드에 저장.
	//테스크는 월드 좌표?가 없음 부모로 부터 받자.
	
	// 랜덤 위치 선택을 위한 내비게이션 시스템 얻어오기.
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(ControllingPawn->GetWorld());
	if (!NavSystem)
	{
		return EBTNodeResult::Failed;
	}

	// AI 인터페이스로 형변환.
	IABCharacterAIInterface* AIPawn = Cast< IABCharacterAIInterface>(ControllingPawn);
	if (!AIPawn)
	{
		return EBTNodeResult::Failed;
	}

	// 랜덤 위치를 선택해서 블랙보드에 저장
	FVector Origin = OwnerComp.GetBlackboardComponent()->GetValueAsVector(
		BBKEY_HOMEPOS
	);
	FNavLocation NextPatrolPos;

	// AIPawn에서 정찰 반경 값 얻어오기.
	float PatrolRadius = AIPawn->GetAIPatrolRadius();

	bool SelectPositionResult = NavSystem->GetRandomPointInNavigableRadius(Origin, 500.0f, NextPatrolPos);
	// 랜덤 위치 선택에 성공했으면 블랙보드에 값 저장.
	if (SelectPositionResult)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(BBKEY_PATROLPOS,
			NextPatrolPos.Location);

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
