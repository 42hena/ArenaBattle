// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ABPlayerController.h"

void AABPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 입력 모드 전환.
	// 게임 전용 입력 모드로 설정.
	// 기본 입력 상태가 뷰포트에 흡수되도록.
	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}
