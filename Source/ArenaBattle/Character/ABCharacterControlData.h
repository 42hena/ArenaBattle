// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ABCharacterControlData.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABCharacterControlData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UABCharacterControlData();

public:
	UPROPERTY(EditAnywhere, Category = Pawn)
	uint8 bUseControllerRotationPtich : 1;

	UPROPERTY(EditAnywhere, Category = Pawn)
	uint8 bUseControllerRotationYaw : 1;

	UPROPERTY(EditAnywhere, Category = Pawn)
	uint8 bUseControllerRotationRoll : 1;

	// Character
	UPROPERTY(EditAnywhere, Category = CharacterMovement)
	FRotator RotationRate;

	UPROPERTY(EditAnywhere, Category = CharacterMovement)
	uint8 bUseControllerDesiredRotation : 1;

	UPROPERTY(EditAnywhere, Category = CharacterMovement)
	uint8 bOrientRotationToMovement : 1;

	// Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputMappingContext> InputMappingContext;

	// SpringArm
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SpringArm)
	float TargetArmLength;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SpringArm)
	FRotator RelativeRotation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SpringArm)
	uint32 bDoCollisionTest : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SpringArm)
	uint32 bUsePawnControlRotation : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SpringArm)
	uint32 bInheritPitch : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SpringArm)
	uint32 bInheritYaw : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SpringArm)
	uint32 bInheritRoll : 1;
};
