// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ABComboActionData.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABComboActionData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UABComboActionData();

public:
	// 몽타주 섹션 이름 접두사
	UPROPERTY(EditAnywhere, Category = Name)
	FString MontageSectionNamePrifix;

	// 최대 콤보 개수 (4개)
	UPROPERTY(EditAnywhere, Category = ComboData)
	uint32 MaxComboCount;

	// 프레임 재생 속도(애니메이션 에셋에서 확인)
	UPROPERTY(EditAnywhere, Category = ComboData)
	float FrameRate;

	// 콤보 공격 마다의 콤보 판정이 되는 프레임 값.
	// 입력이 이 프레임 전에 잘 들어왔는지 확인.
	UPROPERTY(EditAnywhere, Category = ComboData)
	TArray<float> EffectiveFrameCount;
};
