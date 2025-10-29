// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ABCharacterBase.h"

#include "Engine/StreamableManager.h"

#include "ABCharacterNonPlayer.generated.h"

/**
 * 
 */
UCLASS(config=ArenaBattle)
class ARENABATTLE_API AABCharacterNonPlayer : public AABCharacterBase
{
	GENERATED_BODY()
public:
	AABCharacterNonPlayer();

protected:
	virtual void PostInitializeComponents() override;

protected:
	virtual void SetDead() override;

	// config 파일에 설정된 경로 값을 배열로 관리.
	// 
	UPROPERTY(config)
	TArray<FSoftObjectPath> NPCMeshes;

	// 애샛 비동기 로드에 필요한 핸들.
	TSharedPtr<FStreamableHandle> NPCMeshHandle;

	// 비동기로 요청한 메시 로드가 완료되면 호출될 콜백 함수.
	void NPCMeshLoadCompleted();
};
