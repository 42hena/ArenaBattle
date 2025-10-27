// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ABHpBarWidget.h"

#include "Components/ProgressBar.h"

UABHpBarWidget::UABHpBarWidget(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	// 캐릭터를 통해서 제대로 설정되어야 함.

	// 시작할 때 음수로 설정해서 나중에 사용할 때 제대로 값이 설정됐는지 확인.
	MaxHp = -1.0f;
}

void UABHpBarWidget::UpdateHpBar(float NewCurrentHp)
{
	// 값 검증
	ensureAlways(MaxHp > 0.0f);

	// 프로그래스 바 설정 여부 확인
	if (HpProgressBar)
	{
		HpProgressBar->SetPercent(NewCurrentHp / MaxHp);
	}
}

void UABHpBarWidget::NativeConstruct()
{
	Super:: NativeConstruct();

	// 위젯 참조를 위한 코드 작성.
	HpProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PbHpBar")));
	ensureAlways(HpProgressBar);

}
