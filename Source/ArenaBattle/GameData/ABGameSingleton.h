// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ABCharacterStat.h"
#include "ABGameSingleton.generated.h"

// 커스텀 로그 카테고리 추가
DECLARE_LOG_CATEGORY_EXTERN(LogABGameSingleton, Error, All);

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABGameSingleton : public UObject
{
	GENERATED_BODY()
	
public:
	UABGameSingleton();

	// 싱글톤 접근을 위한 Getter
	static UABGameSingleton& Get();

	// 레벨을 사용해 스탯 데이터를 반환하는 함수
	FORCEINLINE FABCharacterStat GetCharacterStat(int32 InLevel) const
	{
		// 인덱스 유효성 검증.
		bool IsValid = CharacterStatTable.IsValidIndex(InLevel - 1);
		return IsValid ? CharacterStatTable[InLevel - 1] : FABCharacterStat();
	}

	UPROPERTY()
	int32 CharacterMaxLevel;



private:
	TArray<FABCharacterStat> CharacterStatTable;
};
