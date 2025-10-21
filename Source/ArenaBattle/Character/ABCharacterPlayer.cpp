// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterPlayer.h"

#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"


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
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple.SKM_Manny_Simple'"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

	// Animation 지정
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimRef(TEXT("/Script/Engine.AnimBlueprint'/Game/Characters/Mannequins/Anims/Unarmed/ABP_Unarmed.ABP_Unarmed_C'"));
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
}

void AABCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();
}

void AABCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 
}

void AABCharacterPlayer::Move(const FInputActionValue& Value)
{
}

void AABCharacterPlayer::Look(const FInputActionValue& Value)
{
}
