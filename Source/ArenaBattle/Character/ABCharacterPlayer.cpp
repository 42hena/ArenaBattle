// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterPlayer.h"

#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "InputMappingContext.h"
#include "InputAction.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "ABCharacterControlData.h"

#include "InputActionValue.h"


AABCharacterPlayer::AABCharacterPlayer()
{
	// 기본 컴포넌트 설정.
	// 컨트롤러 회전 값을 받아서 사용하는 옵션 비활성화
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;	// 기본 true값

	// 무브먼트 설정
	// 이동할 때 이동하는 방향과 회전 방향을 맞추는 옵션 설정.
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);	// X, Y, Z가 아니다!!! -> (Y, Z, X)!!!
	GetCharacterMovement()->JumpZVelocity = 700.0f;

	// Capsule 컴포넌트 설정은 루트이므로 설정할 수 없음.
	// GetCapsuleComponent()->SetRelativeLocationAndRotation(FVector(), FRotator());

	// 매시 컴포넌트 설정
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));

	// 애샛 지정
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Warrior.SK_CharM_Warrior'"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

	// Animation 지정
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimRef(TEXT("/Script/Engine.AnimBlueprint'/Game/ArenaBattle/Animation/ABP_ABCharacter.ABP_ABCharacter_C'"));
	if (AnimRef.Class)
	{
		// 삭제되었음.
		//GetMesh()->SetAnimClass(AnimRef.Class);
		GetMesh()->SetAnimInstanceClass(AnimRef.Class);
	}

	// SpringArm 컴포넌트 생성
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));

	// SpringArm 컴포넌트 설정
	SpringArm->SetupAttachment(RootComponent, NAME_None);
	SpringArm->TargetArmLength = 500.0f;
	SpringArm->bUsePawnControlRotation = true;	// Controller랑 SpringArm과 동기화

	// Camera 컴포넌트 생성
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	
	// Camera 설정.
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);


	// [입력 설정]
	// Input의 MoveAction 설정
	static ConstructorHelpers::FObjectFinder<UInputAction> ShoulderMoveActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_ShoulderMove.IA_ShoulderMove'"));
	if (ShoulderMoveActionRef.Succeeded())
	{
		ShoulderMoveAction = ShoulderMoveActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> QuaterMoveActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_QuaterMove.IA_QuaterMove'"));
	if (QuaterMoveActionRef.Succeeded())
	{
		QuaterMoveAction = QuaterMoveActionRef.Object;
	}

	// Input의 LookAction 설정
	static ConstructorHelpers::FObjectFinder<UInputAction> ShoulderLookActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_ShoulderLook.IA_ShoulderLook'"));
	if (ShoulderLookActionRef.Succeeded())
	{
		ShoulderLookAction = ShoulderLookActionRef.Object;
	}

	// Input의 JumpAction 설정
	static ConstructorHelpers::FObjectFinder<UInputAction> JumpActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_Jump.IA_Jump'"));
	if (JumpActionRef.Succeeded())
	{
		JumpAction = JumpActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> ChangeControlActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_ChangeControl.IA_ChangeControl'"));
	if (ChangeControlActionRef.Succeeded())
	{
		ChangeControlAction = ChangeControlActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> AttackActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_Attack.IA_Attack'"));
	if (AttackActionRef.Succeeded())
	{
		AttackAction = AttackActionRef.Object;
	}

	CurrentCharacterType = ECharacterControlType::QuaterView;
}

void AABCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	SetCharacterControl(CurrentCharacterType);
}

void AABCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 해당하는 Action에 함수를 Bind하기.
	
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent)
	{
		// 입력 바인딩(연결).

		// 우리가 만든 함수 매핑
		EnhancedInputComponent->BindAction(ShoulderMoveAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::ShoulderMove);
		EnhancedInputComponent->BindAction(QuaterMoveAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::QuaterMove);
		EnhancedInputComponent->BindAction(ShoulderLookAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::ShoulderLook);
		
		// 뷰 변경 하는 거 매핑
		EnhancedInputComponent->BindAction(ChangeControlAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::ChangeCharacterControl);

		// UE에서 만들어진 함수 매핑
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::Attack);		
	}

}

void AABCharacterPlayer::ChangeCharacterControl()
{
	if (CurrentCharacterType == ECharacterControlType::QuaterView)
	{
		SetCharacterControl(ECharacterControlType::ShoulderView);
	}
	else if (CurrentCharacterType == ECharacterControlType::ShoulderView)
	{
		SetCharacterControl(ECharacterControlType::QuaterView);
	}
	else
	{
		// 예외 처리.
		UE_LOG(LogTemp, Log, TEXT("Oh my God"));
	}
}

void AABCharacterPlayer::ShoulderMove(const FInputActionValue& Value)
{
	// 입력 값 읽어오기
	FVector2D Movement= Value.Get<FVector2D>();
	
	// 이동할 방향 만들기
	// 카메라가 바라보는 방향(컨트롤러의 방향)을 기준으로 이동 방향 만들기.
	FRotator Rotation = GetControlRotation();
	FRotator YawRotation = FRotator(0, Rotation.Yaw, 0);	// (Pitch, Yaw, Roll)

	// 앞 방향 
	FVector ForwardVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	// 오른쪽 방향
	FVector RightVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// movement에 입력 전달하기.
	// 크기가 1인 숫자를 곱하는 행위는 "회전"을 의미!!!
	AddMovementInput(ForwardVector, Movement.Y);
	AddMovementInput(RightVector, Movement.X);

}

void AABCharacterPlayer::QuaterMove(const FInputActionValue& Value)
{
	// 입력 값 읽어오기
	FVector2D Movement = Value.Get<FVector2D>();

	// 이동할 방향 만들기
	// 카메라가 바라보는 방향(컨트롤러의 방향)을 기준으로 이동 방향 만들기.
	FVector MoveDirection(Movement.Y, Movement.X, 0.0f);	// 입력은 상하가 y 언리얼은 X...

	// 입력의 크기
	float MovementVectorSize = Movement.Size();

	// 이동 단위 벡터 만들기.
	MoveDirection = MoveDirection.GetSafeNormal();

	// 케릭터가 이동하는 방향에 맞게 컨트롤러 회전 설정.
	Controller->SetControlRotation(
		FRotationMatrix::MakeFromX(MoveDirection).Rotator()
	);

	AddMovementInput(MoveDirection, MovementVectorSize);
}

void AABCharacterPlayer::ShoulderLook(const FInputActionValue& Value)
{
	// 입력 값 읽어오기
	FVector2D LookValue = Value.Get<FVector2D>();
	
	// 컨트롤러에 회전 적용.
	// 마우스 좌우 드래그 입력을 컨트롤러 Z축 회전(Yaw)에 적용
	AddControllerYawInput(LookValue.X);

	// 마우스 상하 드래그 입력을 컨트롤러 Y축 회전(피치)에 적용
	AddControllerPitchInput(-LookValue.Y);
}

void AABCharacterPlayer::SetCharacterControl(ECharacterControlType NewCharacterControlType)
{
	UABCharacterControlData* NewCharacterControl = CharacterControlManager[NewCharacterControlType];
	check(NewCharacterControl);

	// 데이터 에셋을 전달해 함수 호출.
	SetCharacterControlData(NewCharacterControl);


	// 향상된 입력 시스템 객체 얻어오기
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (IsValid(PlayerController))	// if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* InputSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

		// 사용할 입력 매핑 컨텍스트 등록.
		if (InputSystem)
		{
			// 기존에 설정된 매핑 제거.
			InputSystem->ClearAllMappings();

			// 새로운 입력 매핑 컨텍스트 추가.
			InputSystem->AddMappingContext(NewCharacterControl->InputMappingContext, 0);
		}
	}
	CurrentCharacterType = NewCharacterControlType;
}

// 컨트롤 데이터 설정.
void AABCharacterPlayer::SetCharacterControlData(const UABCharacterControlData* InCharacterControlData)
{
	Super::SetCharacterControlData(InCharacterControlData);
	
	// Spring Arm
	SpringArm->TargetArmLength = InCharacterControlData->TargetArmLength;
	SpringArm->SetRelativeRotation(InCharacterControlData->RelativeRotation);
	SpringArm->bDoCollisionTest = InCharacterControlData->bDoCollisionTest;
	SpringArm->bUsePawnControlRotation = InCharacterControlData->bUsePawnControlRotation;
	SpringArm->bInheritPitch = InCharacterControlData->bInheritPitch;
	SpringArm->bInheritYaw = InCharacterControlData->bInheritYaw;
	SpringArm->bInheritRoll = InCharacterControlData->bInheritRoll;
}


void AABCharacterPlayer::Attack()
{
	UE_LOG(LogTemp, Log, TEXT("Attack"));
	ProcessComboCommand();
}
