// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterBase.h"

#include "ABCharacterControlData.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Character/ABComboActionData.h"
#include "Physics/ABCollision.h"

#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"

#include "CharacterStat/ABCharacterStatComponent.h"
#include "Components/WidgetComponent.h"
#include "UI/ABWidgetComponent.h"

#include "UI/ABHpBarWidget.h"
#include "Item/ABItemData.h"
#include "Item/ABWeaponItemData.h"

// Sets default values
AABCharacterBase::AABCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 컴포넌트 설정
	GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_ABCAPSULE);

	// 메시의 콜리전은 NoCollision으로 설정.
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));

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
	
	// 죽음 몽타주 애셋 설정.
	static ConstructorHelpers::FObjectFinder<UAnimMontage> DeadMontageRef(TEXT("/Game/ArenaBattle/Animation/AM_Dead.AM_Dead"));
	if (DeadMontageRef.Object)
	{
		DeadMontage = DeadMontageRef.Object;
	}

	// Stat 컴포넌트 (Actor 컴포넌트라서 계층 지정 필요 없음.)
	Stat = CreateDefaultSubobject<UABCharacterStatComponent>(TEXT("Stat"));

	// Widget 컴포넌트 (Scene 컴포넌트라서 Root 지정 필요)
	//HpBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	HpBar = CreateDefaultSubobject<UABWidgetComponent>(TEXT("Widget"));
	HpBar->SetupAttachment(GetMesh());

	// 위치 조정.
	HpBar->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f));

	// 어떤 WidgetBlueprint를 사용해 그릴지 지정.
	static ConstructorHelpers::FClassFinder<UUserWidget>HPBarWidgetRef(TEXT("/Game/ArenaBattle/UI/WBP_HpBar.WBP_HpBar_C"));
	if (HPBarWidgetRef.Class)
	{
		// 컴포넌트에서 사용할 위젯 클래스 설정.
		HpBar->SetWidgetClass(HPBarWidgetRef.Class);

		// 위젯을 그릴 공간 지정.
		HpBar->SetWidgetSpace(EWidgetSpace::Screen);	// BP에서는 world로 했고, billboard? 로 했음.

		HpBar->SetDrawSize(FVector2D(150.0f, 15.0f));

		// 충돌할 이유 없음.
		HpBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}


#pragma region ItemSection
	TakeItemActions.Add(FOnTakeItemDelegate::CreateUObject(this, &AABCharacterBase::EquipWeapon));
	TakeItemActions.Add(FOnTakeItemDelegate::CreateUObject(this, &AABCharacterBase::DrinkPotion));
	TakeItemActions.Add(FOnTakeItemDelegate::CreateUObject(this, &AABCharacterBase::ReadScroll));
#pragma endregion

#pragma region WeaponComponent
	// Weapon Component
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));
#pragma endregion
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

void AABCharacterBase::AttackHitCheck()
{
	// 애님 노티파이를 통해 함수가 호출됨.
	// 충돌 판정 로직 작성.

	// 트레이스를 활용해 충돌 검사
	FHitResult OutHitResult;

	// 충돌 판정 시작 위치
	FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
	const float AttackRange = 50.0f;

	// 트레이스에 사용할 반지름 값.
	const float AttackRadius = 50.0f;

	// 프로파일링할 때 쓰는 거 Attack은...

	// SCENE_QUERY_STAT-FName 타입의 태그값 생성 매크로(엔진 내부에서 사용).
	// 두번째 인자: 복잡한 형태로 충돌체를 감지할지 여부 지정.
	// 세번째 인자: 충돌 판정에서 제외할 액터 목록(자기자신 제외).
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);

	FVector End = Start + GetActorForwardVector() * AttackRange;
	bool HitDetected = GetWorld()->SweepSingleByChannel(
		OutHitResult, Start, End, FQuat::Identity, CCHANNEL_ABACTION, FCollisionShape::MakeSphere(AttackRadius), Params);

	// 충돌 판정 종료 위치
	if (HitDetected)
	{
		// 데미지 양
		const float AttackDamage = 30.0f;

		// 데미지 이벤트
		FDamageEvent DamageEvent;

		// 데미지 전달.
		OutHitResult.GetActor()->TakeDamage(AttackDamage, DamageEvent, GetController(), this);
	}


//#if ENABLE_DRAW

	// 캡슐의 중심 위치.
	FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
	

	// 캡슐 높이의 절반 값.
	float CapsuleHalfHeight = AttackRange * 0.5f;

	// 색상 (그리기 색상)
	FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;


	// 충돌 디버그 (시각적 도구 활용)
	DrawDebugCapsule(
		GetWorld(),
		CapsuleOrigin, CapsuleHalfHeight, AttackRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(),
		DrawColor, false, 5.0f
	);
//#endif
}

void AABCharacterBase::SetupCharacterWidget(UABUserWidget* InUserWidget)
{
	// 초기 값 설정 및 델리게이트 연결.
	UABHpBarWidget* HpBarWidget = Cast<UABHpBarWidget>(InUserWidget);
	if (HpBarWidget)
	{
		// 스탯 데이터의 기반으로 위젯에 값 설정.
		HpBarWidget->SetMaxHp(Stat->GetMaxHp());
		HpBarWidget->UpdateHpBar(Stat->GetCurrentHp());

		// 델리게이트 연결
		Stat->OnHpChanged.AddUObject(HpBarWidget, &UABHpBarWidget::UpdateHpBar);
	}
}

void AABCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 체력을 모두 소진했을 때 발행되는 델리게이트에 구독
	Stat->OnHpZero.AddUObject(this, &AABCharacterBase::SetDead);
}

float AABCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float AppliedDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// @Test : 바로 죽음 처리.
	//SetDead();
	Stat->ApplyDamage(DamageAmount);


	return AppliedDamage;
}

void AABCharacterBase::SetDead()
{
	// 케릭터 무브먼트 끄기
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	// 죽는 애니메이션 재생.
	PlayDeadAnimation();

	// 콜리전 끄기.
	SetActorEnableCollision(false);

	// 죽으면 HpBar 사라지도록 처리.
	HpBar->SetHiddenInGame(true);
}

void AABCharacterBase::PlayDeadAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		// 이미 재생중인 몽타주가 있다면 모두 종료
		AnimInstance->StopAllMontages(0.0f);

		const float PlayRate = 1.0f;
		AnimInstance->Montage_Play(DeadMontage, 1.0f);
	}
}

void AABCharacterBase::TakeItem(UABItemData* InItemData)
{
	//UE_LOG(LogTemp, Log, TEXT("ITem 습득: 타입:%d"), (uint8)InItemData->Type);

	// 3가지 종류의 아이템에 따라 처리를 분기
	/*switch (InItemData->Type)
	{
	case EItemType::Weapon:
		EquipWeapon(InItemData);
		break;

	case EItemType::Potion:
		DrinkPotion(InItemData);
		break;

	case EItemType::Scroll:
		ReadScroll(InItemData);
		break;

	default:
		break;
	}*/

	if (InItemData)
	{
		// 타입 별로 인덱스 구하기
		uint8 Index = (uint8)InItemData->Type;

		// 호출할 델리게이트 가져오기
		FOnTakeItemDelegate Delegate = TakeItemActions[Index].ItemDelegate;

		// 델리게이트 호출.
		Delegate.ExecuteIfBound(InItemData);
	}
}

void AABCharacterBase::EquipWeapon(UABItemData* InItemData)
{
	//UE_LOG(LogTemp, Log, TEXT("EquipWeapon"));
	UABWeaponItemData* WeaponItemData = Cast<UABWeaponItemData>(InItemData);
	if (WeaponItemData)
	{
		// 무기 메시가 이미 로드 됐는지 확인.
		if (WeaponItemData->WeaponMesh.IsPending())
		{
			WeaponItemData->WeaponMesh.LoadSynchronous();
			// Async로 가능하긴 함.
		}

		// 강참조 방식
		//Weapon->SetSkeletalMesh(WeaponItemData->WeaponMesh);

		// 로그가 완료 시 메시 설정.
		Weapon->SetSkeletalMesh(WeaponItemData->WeaponMesh.Get());

	}
}

void AABCharacterBase::DrinkPotion(UABItemData* InItemData)
{
	UE_LOG(LogTemp, Log, TEXT("DrinkPotion"));
}

void AABCharacterBase::ReadScroll(UABItemData* InItemData)
{
	UE_LOG(LogTemp, Log, TEXT("ReadScroll"));
}
