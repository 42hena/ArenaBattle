// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "Blueprint/UserWidget.h"
#include "ABUserWidget.h"
#include "ABHpBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABHpBarWidget : // public UUserWidget
	public UABUserWidget
{
	GENERATED_BODY()
	
public:
	UABHpBarWidget(const FObjectInitializer& ObjectInitializer);
	
	// 최대 체력 값 설정 함수.
	FORCEINLINE void SetMaxHp(float NewMaxHp) { MaxHp = NewMaxHp; }


	// HpBar에 거이지 설정을 위해 사용할 함수. Stat 변경 시 Delegate 바인딩할 함수.
	void UpdateHpBar(float NewCurrentHp);

protected:
	// 위젯이 모두 초기화가 된 후에 호출되는 함수.
	virtual void NativeConstruct() override;

protected:
	// PrograssBar 참조 변수
	UPROPERTY()
	TObjectPtr<class UProgressBar> HpProgressBar;	// BP_Widget 연동할 용도.

	// 최대 체력 값(게이지 계산용)
	UPROPERTY()
	float MaxHp;
};
