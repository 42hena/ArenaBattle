// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ABCharacterBase.generated.h"

// 케릭터 컨트롤 타입을 지정하는 열거형
UENUM()
enum class ECharacterControlType
{
	ShoulderView,
	QuaterView
};

UCLASS()
class ARENABATTLE_API AABCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AABCharacterBase();

protected:
	// 컨트롤 데이터 설정
	virtual void SetCharacterControlData(const class UABCharacterControlData* InCharacterControlData);


protected:
	// 열거형 타입 - UABCharacterControlData 에셋을 관리하는 맵
	UPROPERTY(EditAnywhere, Category = CharacterControl, meta = (AllowPirvateAccess = "true"))
	TMap<ECharacterControlType, class UABCharacterControlData*> CharacterControlManager;
};
