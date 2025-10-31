// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ABCharacterStatWidget.h"
#include "ABCharacterStatWidget.h"
#include "GameData/ABCharacterStat.h"

#include "Components/TextBlock.h"

void UABCharacterStatWidget::UpdateStat(const FABCharacterStat& BaseStat, const FABCharacterStat& ModifierStat)
{
	for (TFieldIterator<FNumericProperty> PropIt(FABCharacterStat::StaticStruct()); PropIt != nullptr; ++PropIt)
	{
		// 속성의 키값(이름).
		const FName PropKey(PropIt->GetName());

		// Base 스탯 데이터 변수
		float BaseData = 0.0f;

		PropIt->GetValue_InContainer(reinterpret_cast<const void*>(&BaseStat),
			&BaseData);

		// Modifier 스탯 데이터 변수
		float ModifierData = 0.0f;

		PropIt->GetValue_InContainer(reinterpret_cast<const void*>(&ModifierStat),
			&ModifierData);

		// 텍스트 블록에 값 설정.
		UTextBlock** BaseTextBlockPtr = BaseLookup.Find(PropKey);
		if (BaseTextBlockPtr != nullptr)
		{
			FString BaseDataString = FString::SanitizeFloat(BaseData);
			FText BaseDataText = FText::FromString(BaseDataString);

			// 텍스트 블록에 설정.
			(*BaseTextBlockPtr)->SetText(BaseDataText);
		}

		// 텍스트 블록에 값 설정.
		UTextBlock** ModifierTextBlockPtr = ModifierLookup.Find(PropKey);
		if (ModifierTextBlockPtr != nullptr)
		{
			FString ModifierDataString = FString::SanitizeFloat(ModifierData);
			FText ModifierDataText = FText::FromString(ModifierDataString);

			// 텍스트 블록에 설정.
			(*ModifierTextBlockPtr)->SetText(ModifierDataText);
		}
	}


}

void UABCharacterStatWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 맵 정보 설정.
	// 언리얼 리플렉션 활용.
	// 구조체의 속성 정보를 리플렉션을 활용해서 얻어오기.
	for (TFieldIterator<FNumericProperty> PropIt(FABCharacterStat::StaticStruct()); PropIt != nullptr; ++PropIt)
	{
		// 속성의 키값(이름).
		const FName PropKey(PropIt->GetName());

		// Base 스탯에 대한 텍스트 블록 이름.
		const FName TextBaseControlName = *FString::Printf(TEXT("Txt%sBase"), *PropIt->GetName());

		// Modifier 스탯에 대한 텍스트 블록 이름.
		const FName TextModifierControlName = *FString::Printf(TEXT("Txt%sModifier"), *PropIt->GetName());

		// 이름 값을 사용해 Base 스탯의 텍스트 블록 얻어오기.
		UTextBlock* BaseTextBlock = Cast<UTextBlock>(GetWidgetFromName(TextBaseControlName));
		if (BaseTextBlock)
		{
			// 맵에 추가
			BaseLookup.Add(PropKey, BaseTextBlock);
		}

		// 이름 값을 사용해 Base 스탯의 텍스트 블록 얻어오기.
		UTextBlock* ModifierTextBlock = Cast<UTextBlock>(GetWidgetFromName(TextModifierControlName));
		if (ModifierTextBlock)
		{
			// 맵에 추가
			ModifierLookup.Add(PropKey, ModifierTextBlock);
		}
	}
}
