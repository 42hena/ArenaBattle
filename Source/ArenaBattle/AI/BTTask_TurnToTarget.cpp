// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_TurnToTarget.h"
#include "ABAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ABAI.h"

#include "Interface/ABCharacterAIInterface.h"



UBTTask_TurnToTarget::UBTTask_TurnToTarget()
{
	// 노드 이름 설정.
	NodeName = TEXT("Turn");
}

EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	// NPC가 캐릭터를 바라보는 방향
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (ControllingPawn == nullptr)
	{
		EBTNodeResult::Failed;
	}

	// 캐릭터 구하기
	APawn* TargetPawn = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BBKEY_TARGET));
	if (TargetPawn == nullptr)
	{
		EBTNodeResult::Failed;
	}

	// NPC가 바라볼 방향 구하기
	FVector LookVector = TargetPawn->GetActorLocation() - ControllingPawn->GetActorLocation();

	// DeltaTime
	UWorld* World = ControllingPawn->GetWorld();

	IABCharacterAIInterface* AIPawn = Cast<IABCharacterAIInterface>(ControllingPawn);
	if (AIPawn == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	// 회전 속도
	float TurnSpeed = AIPawn->GetAITurnSpeed();

	// 바로 쓰면 확 돌음.
	FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();

	// 부드럽게 회전 처리(회전 보간)
	FRotator FinalRotator = FMath::RInterpTo(ControllingPawn->GetActorRotation(), TargetRot, World->GetDeltaSeconds(), TurnSpeed);

	ControllingPawn->SetActorRotation(FinalRotator);
	

	return EBTNodeResult::Succeeded;
}
