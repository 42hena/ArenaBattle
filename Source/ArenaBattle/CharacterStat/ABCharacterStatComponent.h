// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GameData/ABCharacterStat.h"

#include "ABCharacterStatComponent.generated.h"

// 체력 변경 관련 이벤트를 알리기 위한 델리게이트.
// 체력이 모두 소진했을 때 발행할 델리게이트
DECLARE_MULTICAST_DELEGATE(FOnHpZeroDelegate);

// 체력이 변경됐을 때 발행할 델리게이트
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHpChangedDelegate, float /* Current Hp */);

// 체력이 변경됐을 때 발행할 델리게이트
// DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStatChangedDelegate, const FABCharacterStat&, /* BaseStat */ const FABCharacterStat& /* ModifierStat */);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStatChangedDelegate, const FABCharacterStat& /*BaseStat*/, const FABCharacterStat& /*ModifierStat*/);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARENABATTLE_API UABCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UABCharacterStatComponent();

protected:
	// 컴포넌트 초기화할 떄 호출되는 이벤트 함수.
	virtual void InitializeComponent() override;

	// Called when the game starts
	//virtual void BeginPlay() override;

public:	
	// FORCEINLINE float GetMaxHp() const { return MaxHp; }
	void SetLevelStat(int32 InNewLevel);
	FORCEINLINE int32 GetCurrentLevel() const { return CurrentLevel; }
	FORCEINLINE int32 GetAttackRadius() const { return AttackRadius; }

	FORCEINLINE const FABCharacterStat& GetBaseStat() const { return BaseStat; }
	FORCEINLINE void SetBaseStat(const FABCharacterStat& InBaseStat) { BaseStat = InBaseStat; OnStatChanged.Broadcast(BaseStat, ModifierStat); }
	FORCEINLINE const FABCharacterStat& GetModifierStat() const { return ModifierStat; }
	FORCEINLINE void SetModifierStat(const FABCharacterStat& InModifierStat) {
		ModifierStat = InModifierStat; 
		OnStatChanged.Broadcast(BaseStat, ModifierStat);
	}
	FORCEINLINE FABCharacterStat GetTotalStat() const { return BaseStat + ModifierStat; }


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
	FOnStatChangedDelegate OnStatChanged;

protected:
	// 최대 체력
	/*UPROPERTY(VisibleInstanceOnly, Category = Stat)
	float MaxHp;*/

	// 현재 체력
	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat)
	float CurrentHp;

	// 공격 범위
	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat, meta = (AllowPrivateAccess = "true"))
	float AttackRadius;

	// 현재 레벨
	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat)
	int32 CurrentLevel;

	// 기본 스탯 데이터
	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat, meta = (AllowPrivateAccess= "true"))
	FABCharacterStat BaseStat;
	
	// 아이템으로부터 획득한 부가 스탯 데이터
	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat, meta = (AllowPrivateAccess= "true"))
	FABCharacterStat ModifierStat;

};
