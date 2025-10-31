// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ABPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "UI/ABHUDWidget.h"

AABPlayerController::AABPlayerController()
{
	// 위젯 블루프린트로부터 생성할 클래스 정보 가져오기.
	static ConstructorHelpers::FClassFinder<UABHUDWidget> ABHUDWidgetRef(TEXT("/Game/ArenaBattle/UI/WBP_ABHUD.WBP_ABHUD_C"));
	if (ABHUDWidgetRef.Class)
	{
		ABHUDWidgetClass = ABHUDWidgetRef.Class;
	}
}

void AABPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 입력 모드 전환.
	// 게임 전용 입력 모드로 설정.
	// 기본 입력 상태가 뷰포트에 흡수되도록.
	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);


	// CreateWidget
	ABHUDWidget = CreateWidget<UABHUDWidget>(this, ABHUDWidgetClass);
	// 문제 없이 생성됐으면 뷰포트에 추가
	if (ABHUDWidget)
	{
		ABHUDWidget->AddToViewport();
	}
}
