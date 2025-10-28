// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ABStageGimmick.generated.h"

UENUM(BlueprintType)
enum class EStageState : uint8
{
	Ready, 
	Fight,
	Reward,
	Next,
};

DECLARE_DELEGATE(FOnStageChangedDelegate);

USTRUCT(BlueprintType)
struct FStageChangedDelegateWrapper
{
	GENERATED_BODY()

	FStageChangedDelegateWrapper(){}
	FStageChangedDelegateWrapper(const FOnStageChangedDelegate& InStageChangedDelegate) :
		StageChangedDelegate(InStageChangedDelegate)
	{}

	FOnStageChangedDelegate StageChangedDelegate;

	void operator()()
	{
		StageChangedDelegate.ExecuteIfBound();
	}
};

UCLASS()
class ARENABATTLE_API AABStageGimmick : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AABStageGimmick();

public:
	// 생성되는 과정에서 호출됨.
	// 배치된 액터의 상태가 변경될 때마다 호출됨.
	virtual void OnConstruction(const FTransform& Transform) override;

#pragma region Stage
protected:
	//입장 처리를 위한 박스 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = Stage, Meta = (AlloPrivateAccess = "true"))
	TObjectPtr<class UStaticMeshComponent> Stage;

	// 입장 처리를 위한 박스 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = Stage, Meta = (AlloPrivateAccess = "true"))
	TObjectPtr<class UBoxComponent> StageTrigger;

	// 스테이지 상태
	UPROPERTY(EditAnywhere, Category = Stage, Meta = (AlloPrivateAccess = "true"))
	EStageState CurrentState;

	// 상태에 따른 처리를 위한 맵
	UPROPERTY()
	TMap<EStageState, FStageChangedDelegateWrapper> StageDelegate;

	// 박스 컴포넌트의 오버랩 이벤트 등록 함수.
	UFUNCTION()
	void OnStageTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void SetState(EStageState NewState);

	// 각 상태에 실행할 함수.
	void SetReady();
	void SetFight();
	void ChooseReward();
	void ChooseNext();
#pragma endregion

#pragma region Gate
protected:	// Gate
	// 문은 4개가 필요함. 방향에 따라 분리하기 위해 Map으로 구성
	UPROPERTY(VisibleAnywhere, Category = Gate, Meta = (AlloPrivateAccess = "true"))
	TMap<FName, TObjectPtr<class UStaticMeshComponent>> Gates;

	// 각 문에 배치할 박스 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = Gate, Meta = (AlloPrivateAccess = "true"))
	TArray<TObjectPtr<class UBoxComponent>> GateTriggers;

	// 각 문의 충돌 이벤트가 발생했을 때 실행될 함수.
	UFUNCTION()
	void OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void OpenAllGates();
	void CloseAllGates();
#pragma endregion

#pragma region FightSection
protected:
	// 대전할 NPC 클래스
	UPROPERTY(EditAnywhere, Category = Fight, Meta = (AlloPrivateAccess = "true"))
	TSubclassOf<class AABCharacterNonPlayer> OpponentClass;

	// 생성까지 대기할 시간.
	UPROPERTY(EditAnywhere, Category = Fight, Meta = (AlloPrivateAccess = "true"))
	float OpponentSpawnTime;
	
	// 타이머 핸들
	FTimerHandle OpponentTimerHandle;

	// NPC 생성 함수(타이머 종료되면 실행)
	void OnOpponentSpawn();

	// NPC가 죽으면 호출될 함수
	UFUNCTION()
	void OnOppoenentDestroyed(AActor* DestroyedActor);
#pragma endregion
};
