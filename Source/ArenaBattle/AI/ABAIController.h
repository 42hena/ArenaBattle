// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ABAIController.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AABAIController();

	// AI 실행하는 함수.
	void RunAI();

	// AI 중지 함수.
	void StopAI();

protected:
	virtual void OnPossess(APawn* InPawn) override;

private:
	// BlackBoard 에셋
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UBlackboardData> BBAsset;

	// BT 에셋
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UBehaviorTree> BTAsset;

};
