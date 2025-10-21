// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ABGameMode.h"

// #include "Player/ABPlayerController.h"

AABGameMode::AABGameMode()
{
	// Set Default Pawn Class 클래스일때는 타입 정보 지우기?
	// static ConstructorHelpers::FClassFinder<APawn> ThirdPersonClassRef(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter.BP_ThirdPersonCharacter_C"));
	static ConstructorHelpers::FClassFinder<APawn> ThirdPersonClassRef(TEXT("/Script/Engine.Blueprint'/Game/ArenaBattle/Blueprint/BP_Character.BP_Character_C'"));
	if (ThirdPersonClassRef.Succeeded())	// ThirdPersonClassRef.Class와 동일
	{
		DefaultPawnClass = ThirdPersonClassRef.Class;
	}

	// 객체는 없고, 타입만 알고 있어서 이렇게 가져옴.
	// PlayerControllerClass = AABPlayerController::StaticClass();

	// 이미 클래스이기 떄문에 _C는 안붙임. 타입이 Class가아니면 붙임.
	// "/Script/CoreUObject.Class'/Script/ArenaBattle.ABPlayerController'" .Class라 되어있네?
	// static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassRef(TEXT("/Script/CoreUObject.Class'/Script/ArenaBattle.ABPlayerController'"));
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassRef(TEXT("/Script/Engine.Blueprint'/Game/ThirdPerson/Blueprints/BP_ThirdPersonPlayerController.BP_ThirdPersonPlayerController_C'"));
	if (PlayerControllerClassRef.Succeeded())
	{
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}
}