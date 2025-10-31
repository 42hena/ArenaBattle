// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ABHUDWidget.h"

#include "ABHpBarWidget.h"
#include "ABCharacterStatWidget.h"
#include "Interface/ABCharacterHUDInterface.h"
#include "GameData/ABCharacterStat.h"

UABHUDWidget::UABHUDWidget(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
}

void UABHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 데이터 연동.
	// HpBar와 Stat 위젯과 연동되도록 해야함.
	HpBar = Cast<UABHpBarWidget>(GetWidgetFromName(TEXT("WidgetHpBar")));
	ensureAlways(HpBar);

	CharacterStat = Cast<UABCharacterStatWidget>(GetWidgetFromName(TEXT("WidgetCharacterStat")));
	ensureAlways(CharacterStat);

	// 인터페이스를 통해서 캐릭터에 UI 설정 요청
	IABCharacterHUDInterface* HUDPawn = Cast<IABCharacterHUDInterface>(GetOwningPlayer()->GetPawn());
	// GetOwningPlayerPawn
	if (HUDPawn)
	{
		HUDPawn->SetupHUDWidget(this);
	}
}

void UABHUDWidget::UpdateHpBar(float NewCurrentHp)
{
	// HpBar에 값 전달.
	HpBar->UpdateHpBar(NewCurrentHp);
}

void UABHUDWidget::UpdateStat(const FABCharacterStat& BastStat, const FABCharacterStat& ModifierStat)
{
	// 최종 스탯 계산
	FABCharacterStat TotalStat = BastStat + ModifierStat;
	
	// HpBar 업데이트
	HpBar->SetMaxHp(TotalStat.MaxHp);

	// 캐릭터 스탯 UI에 새 데이터 전달
	CharacterStat->UpdateStat(BastStat, ModifierStat);
}