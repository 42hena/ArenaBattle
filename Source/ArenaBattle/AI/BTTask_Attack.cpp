// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_Attack.h"
#include "BrainComponent.h"
#include "BehaviorTree\BehaviorTreeComponent.h"
#include "Interface/ABCharacterAIInterface.h"
#include "AIController.h"

UBTTask_Attack::UBTTask_Attack()
{
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

    // 이 태스크가 해야할 일: NPC한테 공격 명령 전달 요청
    APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
    if (ControllingPawn == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    // AI 인터페이스로 변환
    IABCharacterAIInterface* AIPawn = Cast< IABCharacterAIInterface>(ControllingPawn);
    if (AIPawn == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    // NPC에 공격 종료 시 호출할 함수 전달.
    AIPawn->SetAIAttackDelegate(FAICharacterAttackFinished::CreateLambda(
        [&]()
        {
            // 캐릭터의 공격이 끝나면, 태스크 실행 결과를 설정.
            // InProgress로 반환한 뒤에 실제 종료 처리는
            // FinishLatentTask 함수를 활용해서 설정.
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        }));


    // 공격 요청
    AIPawn->AttackByAI();

    return EBTNodeResult::InProgress;
}
