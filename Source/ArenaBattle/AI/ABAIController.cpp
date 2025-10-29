// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/ABAIController.h"

#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BehaviorTree.h"

AABAIController::AABAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBAssetRef(TEXT("/Script/AIModule.BlackboardData'/Game/ArenaBattle/AI/BB_ABCharacter.BB_ABCharacter'"));
	if (BBAssetRef.Succeeded())
	{
		BBAsset = BBAssetRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTAssetRef(TEXT("/Script/AIModule.BehaviorTree'/Game/ArenaBattle/AI/BT_ABCharacter.BT_ABCharacter'"));
	if (BTAssetRef.Succeeded())
	{
		BTAsset = BTAssetRef.Object;
	}
}

void AABAIController::RunAI()
{
	// 블랙보드 컴포넌트 받아오기
	UBlackboardComponent* BlackboardComponent = Blackboard.Get();

	// 블랙보드 사용 설정.
	if (UseBlackboard(BBAsset, BlackboardComponent))
	{
		// 행동트리 실행
		bool Result = RunBehaviorTree(BTAsset);
		
		// 확인
		ensureAlways(Result);
	}
}

void AABAIController::StopAI()
{
	// 실행 중인 BT 컴포넌트 받아오기.
	UBehaviorTreeComponent* BTComp = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (BTComp)
	{
		BTComp->StopTree();
	}
}

void AABAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// 컨트롤러가 폰에 빙의하면 AI가 실행되도록 함수 호출.
	// BT가 실행되면 Pawn이 영향을 받기 때문에 빙의 처리가 완료되어야 함.
	RunAI();
}
