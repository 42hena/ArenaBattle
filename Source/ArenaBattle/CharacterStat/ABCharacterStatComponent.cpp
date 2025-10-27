// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStat/ABCharacterStatComponent.h"

// Sets default values for this component's properties
UABCharacterStatComponent::UABCharacterStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	//PrimaryComponentTick.bCanEverTick = true;

	// 초기값 설정
	MaxHp = 200;
	//CurrentHp = MaxHp;
	SetHp(MaxHp);
}


// Called when the game starts
void UABCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

float UABCharacterStatComponent::ApplyDamage(float InDamage)
{
	// 대미지 처리 계산을 위해 이전 체력 저장.
	const float PrevHp = CurrentHp;
	
	// 대미지 값에 음수가 전달되는 것 방지.
	const float ActualDamage = FMath::Clamp(InDamage, 0.0f, InDamage);

	// 대미지 처리.
	//CurrentHp = FMath::Clamp<float>(PrevHp - ActualDamage, 0.0f, MaxHp);
	SetHp(PrevHp - ActualDamage);

	// 체력을 모두 소진했는지 확인, KINDA_SMALL_NUMBER -> 0으로 취급하는 매우 작은 수
	if (CurrentHp <= KINDA_SMALL_NUMBER)
	{
		// 캐릭터 죽음 처리.
		// 지금 구조에서는 하위 계층인 컴포넌트가 직접 캐릭터에 접근해서
		// 체력이 소진됐음을 알려야 함.
		// 이를 위해 델리게이트를 사용
		OnHpZero.Broadcast();	// 캐릭터에 접근하거나 다른 것에 접근하지 않음...
	}

	return ActualDamage;
}

void UABCharacterStatComponent::SetHp(float NewHp)
{
	// Current = NewHp
	CurrentHp = FMath::Clamp<float>(NewHp, 0.0f, MaxHp);

	// 체력 변경 이벤트 발행.
	OnHpChanged.Broadcast(CurrentHp);
}
