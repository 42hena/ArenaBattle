// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ABCharacterStat.generated.h"

USTRUCT(BlueprintType)
struct FABCharacterStat : public FTableRowBase
{
	GENERATED_BODY()

public:
	FABCharacterStat() :MaxHp(0.0f), Attack(0.0f), AttackRange(0.0f), AttackSpeed(0.0f), MovementSpeed(0.0f) { }

	// 두 구조체를 더하는 오버로딩
	FABCharacterStat operator+(const FABCharacterStat& Other) const
	{
		// 포인터 연산을 활용한 덧셈 처리.
		// 현재 포인터 가져오기
		const float* const ThisPtr = reinterpret_cast<const float* const>(this);
		
		// 덧셈할 데이터의 포인터
		const float* const OtherPtr = reinterpret_cast<const float* const>(&Other);

		// 결과 저장용 변수
		FABCharacterStat Results;
		
		// 결과 저장용 변수의 포인터.
		 float*  ResultPtr = reinterpret_cast<float* >(&Results);

		// 루프 처리를 위해 float 변수의 수 계산.
		int32 StatNum = sizeof(FABCharacterStat) / sizeof(float);

		// 루프를 순회하면서 덧셈 처리
		for (int32 Index = 0; Index < StatNum; ++Index)
		{
			ResultPtr[Index] = ThisPtr[Index] + OtherPtr[Index];
		}

		return Results;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float MaxHp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float Attack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float AttackSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float MovementSpeed;
};


