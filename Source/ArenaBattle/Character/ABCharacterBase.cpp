// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterBase.h"

#include "ABCharacterControlData.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AABCharacterBase::AABCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Map에 데이터 넣기
	// 케릭터 컨트롤 데이터 에셋 검색 후 설정
	static ConstructorHelpers::FObjectFinder<UABCharacterControlData>
		ShoulderDataRef(TEXT("/Script/ArenaBattle.ABCharacterControlData'/Game/ArenaBattle/CharacterControl/ABC_Shoulder.ABC_Shoulder'"));
	if (ShoulderDataRef.Object)
	{
		CharacterControlManager.Add(ECharacterControlType::ShoulderView, ShoulderDataRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UABCharacterControlData>
		QuaterDataRef(TEXT("/Script/ArenaBattle.ABCharacterControlData'/Game/ArenaBattle/CharacterControl/ABC_Quater.ABC_Quater'"));
	if (QuaterDataRef.Object)
	{
		CharacterControlManager.Add(ECharacterControlType::QuaterView, QuaterDataRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage>
		ComboActionMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/ArenaBattle/Animation/AM_ComboAttack.AM_ComboAttack'"));
	if (ComboActionMontageRef.Object)
	{
		ComboActionMontage = ComboActionMontageRef.Object;
	}
}

void AABCharacterBase::SetCharacterControlData(const UABCharacterControlData* InCharacterControlData)
{
	// Pawn부터 설정
	bUseControllerRotationYaw = InCharacterControlData->bUseControllerRotationYaw;

	// CharacterMovement
	GetCharacterMovement()->bOrientRotationToMovement = InCharacterControlData->bOrientRotationToMovement;
	GetCharacterMovement()->bUseControllerDesiredRotation = InCharacterControlData->bUseControllerDesiredRotation;
	GetCharacterMovement()->RotationRate = InCharacterControlData->RotationRate;
}

void AABCharacterBase::ProcessComboCommand()
{
	// 이동 막기(무브먼트 모드를 None으로 설정)
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	// 몽타주 재생.
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		const float AttackSpeedRate = 1.0f;
		// Montage 재생 함수.
		AnimInstance->Montage_Play(ComboActionMontage);

		// 몽타주 재생이 끝나면 특정 함수 실행
		FOnMontageEnded OnMontageEnded;
		OnMontageEnded.BindUObject(this, &AABCharacterBase::ComboActionEnd);
		AnimInstance->Montage_SetEndDelegate(OnMontageEnded, ComboActionMontage);

	}
}

void AABCharacterBase::ComboActionBegin()
{
}

void AABCharacterBase::ComboActionEnd(UAnimMontage* TargetMontage, bool Interrupted)
{
	// 몽타주 재생 끝나면 다시 무브먼트 모드 복구.
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}
