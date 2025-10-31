// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ABCharacterStatWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABCharacterStatWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 스탯 업데이트 함수
	void UpdateStat( const struct FABCharacterStat& BaseStat,  const  struct FABCharacterStat& ModifierStat);
	
protected:
	// 위젯 생성을 보장 받는 초기화 함수
	virtual void NativeConstruct() override;

private:
	// 텍스트 블록 맵 정보.
	UPROPERTY()
	TMap<FName, class UTextBlock*> BaseLookup;
	
	UPROPERTY()
	TMap<FName, class UTextBlock*> ModifierLookup;
};
