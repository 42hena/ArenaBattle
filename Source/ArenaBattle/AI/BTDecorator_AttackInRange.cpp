// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTDecorator_AttackInRange.h"

#include "Interface/ABCharacterAIInterface.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ABAI.h"

UBTDecorator_AttackInRange::UBTDecorator_AttackInRange()
{
	// 노드 이름 설정.
	NodeName = TEXT("CanAttack");
}

bool UBTDecorator_AttackInRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool Result = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	// AIController가 제어하는 폰 정보.
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (ControllingPawn == nullptr)
	{
		return false;
	}

	// 제어하는폰을 AI인터페이스로 변환
	IABCharacterAIInterface* AIPawn = Cast<IABCharacterAIInterface>(ControllingPawn);
	if (AIPawn == nullptr)
	{
		return false;
	}

	// 감지된 폰
	UObject* TargetObject = OwnerComp.GetBlackboardComponent()->GetValueAsObject(BBKEY_TARGET);
	if (TargetObject == nullptr)
	{
		return false;
	}

	// 감지된 Object 타입을 폰으로 변환
	APawn* Target = Cast<APawn>(TargetObject);
	if (Target == nullptr)
	{
		return false;
	}

	// 감지된 캐릭터와 거리 확인
	float DistanceToTarget = ControllingPawn->GetDistanceTo(Target);

	// 공격 범위 가능 범위 값 얻어오기
	float AttackRange = AIPawn->GetAIAttackRange();

	// 캐릭터와의 거리가 공격 가능 범위안에 있는지 확인 후 결과 반환.
	Result = DistanceToTarget <= AttackRange;
	return Result;
}
