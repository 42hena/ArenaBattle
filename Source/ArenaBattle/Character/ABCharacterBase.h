// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/ABAnimationAttackInterface.h"
#include "ABCharacterBase.generated.h"


// 케릭터 컨트롤 타입을 지정하는 열거형
UENUM()
enum class ECharacterControlType
{
	ShoulderView,
	QuaterView
};

UCLASS()
class ARENABATTLE_API AABCharacterBase : 
	public ACharacter,
	public IABAnimationAttackInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AABCharacterBase();

protected:
	// 컨트롤 데이터 설정
	virtual void SetCharacterControlData(const class UABCharacterControlData* InCharacterControlData);

protected:	// Combo Section
	// 공격 처음 시작할 때와 콤보 액션을 진행할 때 실행.
	void ProcessComboCommand();

	// 콤보 액션이 시작될 때 실행.
	void ComboActionBegin();

	// 콤보 공격이 끝날 때 실행되는 함수
	void ComboActionEnd(UAnimMontage* TargetMontage, bool Interrupted);

	// 콤보 타이머 설정 함수
	void SetComboCheckTimer();

	// 콤보 타이밍 처리 함수.
	// 타이머에 설정된 시간이 경과했을 때 실행됨.
	// 이 시간 전에 공격 입력이 제대로 들어왔는지 판단 후 콤보 처리 분기.
	void ComboCheck();

	// 애님 노티파이 기반으로 충돌 판정하는 목적으로 사용.
	virtual void AttackHitCheck() override;

	virtual float TakeDamage(float DamageAmount, 
		struct FDamageEvent const& DamageEvent, 
		class AController* EventInstigator, 
		AActor* DamageCauser) override;

	// 죽음 설정 함수.
	// 데미지 처리 후 체력 소진되면 죽음.
	// 죽음 상태 설정.
	virtual void SetDead();

	// 죽음 애니메이션 재생.
	void PlayDeadAnimation();

protected:
	// 열거형 타입 - UABCharacterControlData 에셋을 관리하는 맵
	UPROPERTY(EditAnywhere, Category = CharacterControl, meta = (AllowPirvateAccess = "true"))
	TMap<ECharacterControlType, class UABCharacterControlData*> CharacterControlManager;

	UPROPERTY(VisibleAnywhere, Category = CharacterControl, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> ComboActionMontage;

	// 콤보 처리를 위한 데이터를 가지는 데이터 애셋
	UPROPERTY(VisibleAnywhere, Category = CharacterControl, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UABComboActionData> ComboActionData;

	// 콤보 단계 추적 변수
	UPROPERTY(VisibleAnywhere, Category = CharacterControl, meta = (AllowPrivateAccess = "true"))
	uint32 CurrentCombo = 0;

	// 콤보 판정에 사용할 타이머
	FTimerHandle ComboTimerHandle;

	// 콤보 점프를 판정할 때 사용할 Boolean 변수
	UPROPERTY(VisibleAnywhere, Category = CharacterControl, meta = (AllowPrivateAccess = "true"))
	bool bHasNextComboCommand = false;
	// uint8 bHasNextComboCommand : 1 = false;

	UPROPERTY(VisibleAnywhere, Category = CharacterControl, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> DeadMontage;
};
