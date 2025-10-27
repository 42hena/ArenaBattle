// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ABCharacterStatComponent.generated.h"

// 체력 변경 관련 이벤트를 알리기 위한 델리게이트.
// 체력이 모두 소진했을 때 발행할 델리게이트
DECLARE_MULTICAST_DELEGATE(FOnHpZeroDelegate);

// 체력이 변경됐을 때 발행할 델리게이트
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHpChangedDelegate, float /* Current Hp */);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARENABATTLE_API UABCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UABCharacterStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	FORCEINLINE float GetMaxHp() const { return MaxHp; }
	FORCEINLINE float GetCurrentHp() const { return CurrentHp; }

	// 대미지 적용 함수
	float ApplyDamage(float InDamage);

protected:
	// 체력이 변경됐을 때 실행할 함수
	void SetHp(float NewHp);

public:
	// 발행할 이벤트
	FOnHpZeroDelegate OnHpZero;
	FOnHpChangedDelegate OnHpChanged;

protected:
	// 최대 체력
	UPROPERTY(VisibleInstanceOnly, Category = Stat)
	float MaxHp;

	// 현재 체력
	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat)
	float CurrentHp;
};
