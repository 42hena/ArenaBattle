// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterBase.h"

#include "ABCharacterControlData.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Character/ABComboActionData.h"

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

	// 콤보 액션 데이터 애샛 설정.
	static ConstructorHelpers::FObjectFinder<UABComboActionData> ComboActionDataRef(
		TEXT("/Game/ArenaBattle/ComboAction/ABA_ComboAction.ABA_ComboAction"));
	if (ComboActionDataRef.Object)
	{
		ComboActionData = ComboActionDataRef.Object;
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
	// 처음 공격할 때 처리.
	// 몽타주 재생 요청.

	if (CurrentCombo == 0)
	{
		ComboActionBegin();
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("ProcessComboCommand"));
	// 공격이 이미 재생 중일 때는 타이머를 확인해서 콤보 타이밍 확인.
	// 타이머 핸들이 유효한지 여부로 판단 가능.
	if (!ComboTimerHandle.IsValid())
	{
		bHasNextComboCommand = false;
	}
	else
	{
		bHasNextComboCommand = true;
	}
}

void AABCharacterBase::ComboActionBegin()
{
	
	// 현재 콤보 단계를 1단계로 설정
	CurrentCombo = 1;

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

		// 콤보 타이밍 확인용 타이머 설정
		// 기존에 설정된 타이머 핸들 무효화(초기화)
		ComboTimerHandle.Invalidate();

		// 타이머 설정 및 콤보 단계 처리.
		SetComboCheckTimer();
	}

	// 타이머 동작.
}

void AABCharacterBase::ComboActionEnd(UAnimMontage* TargetMontage, bool Interrupted)
{
	ensureAlways(CurrentCombo != 0);

	// 콤보 데이터 리셋
	CurrentCombo = 0;

	// 몽타주 재생 끝나면 다시 무브먼트 모드 복구.
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void AABCharacterBase::SetComboCheckTimer()
{
	int32 ComboIndex = CurrentCombo - 1;
	
	ensureAlways(ComboActionData->EffectiveFrameCount.IsValidIndex(ComboIndex));

	UABComboActionData *Combo = ComboActionData;
	const float AttackSpeedRate = 1.0f;
	// float ComboEffectTime = (ComboActionData->EffectiveFrameCount[ComboIndex] / ComboActionData->FrameRate / AttackSpeedRate);
	float ComboEffectTime = (ComboActionData->EffectiveFrameCount[ComboIndex] / ComboActionData->FrameRate);


	// 타이머 설정.
	if (ComboEffectTime > 0)
	{
		// 시간은 월드가 관리(시간 관리자를 통해)
		GetWorld()->GetTimerManager().
			SetTimer(ComboTimerHandle, this, &AABCharacterBase::ComboCheck, ComboEffectTime, false);
	}
}

void AABCharacterBase::ComboCheck()
{
	// 타이머 핸들 초기화(재사용을 위해)
	ComboTimerHandle.Invalidate();

	// 콤보 타이머 시간 전에 입력이 들어왔는지 확인.
	if (bHasNextComboCommand)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, ComboActionData->MaxComboCount);

			// 섹션 이름 만들기.
			FName NextSecion = *FString::Printf(TEXT("%s%d"), *ComboActionData->MontageSectionNamePrifix, CurrentCombo);
	
			// 몽타주 점프.
			AnimInstance->Montage_JumpToSection(NextSecion, ComboActionMontage);
			
			// 타이머 재설정
			SetComboCheckTimer();

			// 콤보 처리에 사용한 입력 값도 초기화.
			bHasNextComboCommand = false;
		}
	}
}
