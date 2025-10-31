// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ABHpBarWidget.h"

#include "Components/ProgressBar.h"

#include "Interface/ABCharacterWidgetInterface.h"
#include "Components/TextBlock.h"

UABHpBarWidget::UABHpBarWidget(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	// 캐릭터를 통해서 제대로 설정되어야 함.

	// 시작할 때 음수로 설정해서 나중에 사용할 때 제대로 값이 설정됐는지 확인.
	MaxHp = -1.0f;
}

void UABHpBarWidget::UpdateHpBar(float NewCurrentHp)
{
	// 현재 체력 값 업데이트.
	CurrentHp = NewCurrentHp;

	// 값 검증
	ensureAlways(MaxHp > 0.0f);

	// 프로그래스 바 설정 여부 확인
	if (HpProgressBar)
	{
		HpProgressBar->SetPercent(NewCurrentHp / MaxHp);
	}

	// HP 텍스트 업데이트
	if (HpStat)
	{
		HpStat->SetText(FText::FromString(GetHpStatText()));
	}
}

void UABHpBarWidget::UpdateStat(const FABCharacterStat& BaseStat, const FABCharacterStat& ModifierStat)
{
	// 최대 체력 업데이트
	MaxHp = (BaseStat + ModifierStat).MaxHp;

	// HP 상태 업데이트
	if (HpProgressBar)
	{
		HpProgressBar->SetPercent(CurrentHp / MaxHp);
	}
	// HP 텍스트 업데이트
	if (HpStat)
	{
		HpStat->SetText(FText::FromString(GetHpStatText()));
	}
}

FString UABHpBarWidget::GetHpStatText()
{
	return FString::Printf(TEXT("%.0f / %.0f"), CurrentHp, MaxHp);
}

void UABHpBarWidget::NativeConstruct()
{
	Super:: NativeConstruct();

	// 위젯 참조를 위한 코드 작성.
	HpProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PbHpBar")));
	ensureAlways(HpProgressBar);

	HpStat = Cast<UTextBlock>(GetWidgetFromName(TEXT("TxtHpStat")));
	ensureAlways(HpStat);

	// 인터페이스를 통해 캐릭터에 접근해서
	// 초기화 요청
	IABCharacterWidgetInterface* CharacterWidget = Cast<IABCharacterWidgetInterface>(OwningActor);
	if (CharacterWidget)
	{
		CharacterWidget->SetupCharacterWidget(this);
	}
}
