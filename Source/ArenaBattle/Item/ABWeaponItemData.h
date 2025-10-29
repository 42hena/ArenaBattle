// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ABItemData.h"
#include "GameData/ABCharacterStat.h"
#include "ABWeaponItemData.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABWeaponItemData : public UABItemData
{
	GENERATED_BODY()
	
public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("ABItemData", GetFName());	// GetFName() 은 UABItemData 클래스 이름을 가져옴.
	}

public:
	UPROPERTY(EditAnyWhere, Category = Weapon)
	TSoftObjectPtr<class USkeletalMesh> WeaponMesh;
	//TObjectPtr<class USkeletalMesh> WeaponMesh;

	UPROPERTY(EditAnyWhere, Category = Stat)
	FABCharacterStat ModifierStat;
};
