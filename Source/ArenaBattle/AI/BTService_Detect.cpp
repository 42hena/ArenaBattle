// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_Detect.h"
#include "AIController.h"
#include "Physics/ABCollision.h"
#include "Interface/ABCharacterAIInterface.h"
#include "Engine/OverlapResult.h"
#include "CollisionQueryParams.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ABAI.h"

UBTService_Detect::UBTService_Detect()
{
	// 에디터에서 보이는 이름 설정
	NodeName = TEXT("Detect");

	// TickNode 함수 실행 간격 설정 (단위: 초).
	Interval = 1.0f;
}

void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// 폰에 접근하면 안전.
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (ControllingPawn == nullptr)
	{
		return;
	}

	// AI 인터페이스로 형변환
	IABCharacterAIInterface* AIPawn = Cast<IABCharacterAIInterface>(ControllingPawn);
	if (AIPawn == nullptr)
	{
		return;
	}

	// 감지 범위(반지름) 얻어오기
	float DetectRadius = AIPawn->GetAIDetectRange();

	// 감지 처리를 위해 필요한 값 구하기.
	UWorld* World = ControllingPawn->GetWorld();
	FVector Center = ControllingPawn->GetActorLocation();

	// 충돌 결과 배열 선언
	TArray<FOverlapResult> Results;


	FCollisionQueryParams Params(SCENE_QUERY_STAT(Detect), false, ControllingPawn);
	// 오버랩 함수를 활용해 시야 감지.
	bool Result = World->OverlapMultiByChannel(Results, Center, FQuat::Identity, CCHANNEL_ABACTION, FCollisionShape::MakeSphere(DetectRadius), Params);

	// 충돌 감지됐을 때 처리.
	if (Result)
	{
		for (const auto& OverlapResult : Results)
		{
			APawn* Pawn = Cast<APawn>(OverlapResult.GetActor());
			// 감지된 액터가 폰이고,
			// 해당 폰을 제어하는 컨트롤러가 플레이어 컨트롤러라면,
			// 로직 진행.
			// -> 플레이어 캐릭터인지 확인.
			if (Pawn && Pawn->GetController()->IsPlayerController())
			{
				// 감지된 폰 정보를 블랙보드에 저장
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(BBKEY_TARGET, Pawn);

				// 감지한 정보를 시각적으로 보여주기
				DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Green, false, 0.5f);

				// 감지된 지점(캐릭터 위치) 점으로 표시
				DrawDebugPoint(World, Pawn->GetActorLocation(), 10.0f, FColor::Green, false, 0.5f);
				
				// 감지된 방향을 선으로 표시
				DrawDebugLine(World, ControllingPawn->GetActorLocation(), Pawn->GetActorLocation(), FColor::Green, false, 0.5f);
				return;
			}
		}
	}

	// 여기에 도달했다면, 플레이어 감지에 실패
	// 타겟 액터를 null로 저장
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(BBKEY_TARGET, nullptr);

	// 감지 영역 빨간색으로 표시
	DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Red, false, 0.5f);
}
