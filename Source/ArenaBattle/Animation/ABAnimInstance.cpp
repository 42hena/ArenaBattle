// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/ABAnimInstance.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UABAnimInstance::UABAnimInstance()
{
	// 이동을 판단할 때 사용할 기준 값 설정.
	MovingThreshold = 3.0f;

	// 점프 중인지 판단할 기준 값 설정
	JumpingThreshold = 100.0f;
}

void UABAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// 계속 사용할 참조 값 설정.
	Owner = Cast<ACharacter>(GetOwningActor());
	if (Owner != nullptr)
	{
		Movement = Owner->GetCharacterMovement();
	}
}

void UABAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Movement)
	{
		// 캐릭터 무브먼트 컴포넌트의 현재 속도(방향 빠르기)
		Velocity = Movement->Velocity;

		// 지면에서의 이동 빠르기 구하기.
		GroundSpeed = Velocity.Size2D();

		// 이동 / 정지 상태 설정
		bIsIdle = GroundSpeed < MovingThreshold;

		// 공중에 떠있는지 확인(Movement를 통해)
		bIsFalling = Movement->IsFalling();

		// 점프 중인지 판단 (Z의 속도가 기준점보다 높으면 된다.)
		bIsJumping = bIsFalling & (Velocity.Z > JumpingThreshold);
	}
}
