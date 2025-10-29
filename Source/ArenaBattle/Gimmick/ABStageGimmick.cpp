// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/ABStageGimmick.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

#include "Physics/ABCollision.h"

#include "Character/ABCharacterNonPlayer.h"
#include "Engine/OverlapResult.h"
#include "Item/ABItemBox.h"

// Sets default values
AABStageGimmick::AABStageGimmick()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Stage Section
	Stage = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Stage"));

	// Todo: 스테이지 메시 에셋 설정.
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StageMeshRef(TEXT("/Game/ArenaBattle/Environment/Stages/SM_SQUARE.SM_SQUARE"));
	if (StageMeshRef.Object)
	{
		Stage->SetStaticMesh(StageMeshRef.Object);
	}
	
	// 박스 콜리전 컴포넌트 생성.
	StageTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("StageTriiger"));
	StageTrigger->SetupAttachment(Stage);

	// Todo: 박스 콜리전 크기 설정
	StageTrigger->SetBoxExtent(FVector(775, 775, 300));

	// Todo: 상대 위치 설정
	StageTrigger->SetRelativeLocation(FVector(0.0f, 0.0f, 300.0f));

	
	// Todo: 콜리전 설정
	StageTrigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);
	// Todo: 오버랩 이벤트에 함수 등록
	StageTrigger->OnComponentBeginOverlap.AddDynamic(this, &AABStageGimmick::OnStageTriggerBeginOverlap);

	// Gate Section
	static FName GateSockets[] = {
		TEXT("+XGate"),
		TEXT("-XGate"),
		TEXT("+YGate"),
		TEXT("-YGate"),
	};

	// Todo: 메시 로드 및 설정
	static ConstructorHelpers::FObjectFinder<UStaticMesh> GateMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/ArenaBattle/Environment/Props/SM_GATE.SM_GATE'"));
	

	
	for (const FName& GateSocket : GateSockets)
	{
		// 컴포넌트 생성
		UStaticMeshComponent* Gate = CreateDefaultSubobject<UStaticMeshComponent>(GateSocket);

		if (GateMeshRef.Object)
		{
			Gate->SetStaticMesh(GateMeshRef.Object);
		}

		// 계층 설정
		Gate->SetupAttachment(Stage, GateSocket);

		// 상대 위치 설정.
		Gate->SetRelativeLocation(FVector(0, -80, 0));

		// 상대 회전 설정.
		Gate->SetRelativeRotation(FRotator(0, -90, 0));	// R, P, Y임.

		// 배열에 추가
		Gates.Add(GateSocket, Gate);

		// Gate 콜리전 추가
		//FName TriggerName = *GateSocket.ToString().Append(TEXT("Trigger"));
		FName TriggerName = *FString::Printf(TEXT("%sTrigger"), *GateSocket.ToString());
		UBoxComponent* GateTrigger = CreateDefaultSubobject<UBoxComponent>(TriggerName);
		
		// 계층 설정
		GateTrigger->SetupAttachment(Stage, GateSocket);
		
		// Todo: 크기 지정.
		GateTrigger->SetBoxExtent(FVector(100.0f, 100.0f, 300.0f));

		// Todo: 상대 위치 설정.
		GateTrigger->SetRelativeLocation(FVector(0.0f, 0.0f, 300.0f));

		// 콜리전 설정
		GateTrigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);

		// 오버랩 이벤트에 함수 등록
		GateTrigger->OnComponentBeginOverlap.AddDynamic(this, &AABStageGimmick::OnGateTriggerBeginOverlap);

		// 태그 추가.
		GateTrigger->ComponentTags.Add(GateSocket);

		// 배열에 추가.
		GateTriggers.Add(GateTrigger);

	}

	CurrentState = EStageState::Ready;

	// 델리게이트 맵 설정
	StageDelegate.Add(EStageState::Ready, FOnStageChangedDelegate::CreateUObject(this, &AABStageGimmick::SetReady));
	StageDelegate.Add(EStageState::Fight, FOnStageChangedDelegate::CreateUObject(this, &AABStageGimmick::SetFight));
	StageDelegate.Add(EStageState::Reward, FOnStageChangedDelegate::CreateUObject(this, &AABStageGimmick::ChooseReward));
	StageDelegate.Add(EStageState::Next, FOnStageChangedDelegate::CreateUObject(this, &AABStageGimmick::ChooseNext));


	// Fight Section

	// 생성할 NPC 클래스 정보 설정.
	OpponentClass = AABCharacterNonPlayer::StaticClass();

	// NPC 생성할 때 대기할 시간 값 설정(3.0초)
	OpponentSpawnTime = 3.0f;

	// Reward Section
	// 생성할 아이템 박스의 클래스 정보 설정.
	RewardBoxClass = AABItemBox::StaticClass();

	// 아이템 상자 생성할 위치 저장.
	for (const FName& GateSocket : GateSockets)
	{
		// 문 생성 위치를 반으로 나눈 위치값을 
		// 아이템 상자 생성 위치로 사용
		const FVector BoxLocation = Stage->GetSocketLocation(GateSocket) / 2;

		// 생성 위치를 저장.
		RewardBoxLocations.Add(GateSocket, BoxLocation);
	}
}

void AABStageGimmick::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	SetState(CurrentState);
}

void AABStageGimmick::OnStageTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 캐릭터가 스테이지에 입장하면 대전 상태로 전환.
	SetState(EStageState::Fight);
}

void AABStageGimmick::SetState(EStageState NewState)
{
	// 전달된 상태에 따라 처리할 로직을 분기.

	CurrentState = NewState;
	if (StageDelegate.Contains(CurrentState))
	{
		//StageDelegate[CurrentState].StageChangedDelegate.ExecuteIfBound();
		StageDelegate[CurrentState]();
	}
}

void AABStageGimmick::OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 부딪힌 문을 확인해서 적절한 위치에 새로운 스테이지 기믹 액터 생성.
	

	// 스테이지 기믹 액터 생성 위치
	// 태그 값 활용해서 생성할 위치 가져오기


	// 태그 있는지 확인
	ensureAlways(OverlappedComponent->ComponentTags.Num() == 1);

	FName ComponentTag = OverlappedComponent->ComponentTags[0];

	// 아래 함수를 활용하는 것도 가능.
	// 하지만 일반적으로 액터(객체)나 컴포넌트의 이름을 그대로 로직에 사용하는 것은 지양함.
	// OverlappedComponent->GetName();

	// 생성할 위치를 가진 소켓 이름 값 가져오기.
	// ComponentTag에는 +XGateTrigger와 같은 값이 설정됨.
	FName SocketName = FName(*ComponentTag.ToString().Left(2));
	
	// 소켓이 있는지 확인
	ensureAlways(Stage->DoesSocketExist(SocketName));

	// 생성 위치
	FVector NewLocation = Stage->GetSocketLocation(SocketName);

	// 생성하려는 위치에 스테이지가 이미 있는지 확인.
	// 오버랩으로 검사.
	//TArray<struct FOverlapResult>& OutOverlaps, 
	// const FVector& Pos, 
	// const FQuat& Rot, const FCollisionObjectQueryParams& ObjectQueryParams, 
	// const FCollisionShape& CollisionShape, 
	// const FCollisionQueryParams& Params /* = FCollisionQueryParams::DefaultQueryParam */
	// 
	//

	TArray< FOverlapResult> Results;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(GateTrigger), false, this);
	bool Result = GetWorld()->OverlapMultiByObjectType(Results, NewLocation, FQuat::Identity, FCollisionObjectQueryParams::InitType::AllStaticObjects,
		FCollisionShape::MakeSphere(775.0f), Params);
	if (Result == false)
	{
		// 새로운 스테이지 액터 생성.
		GetWorld()->SpawnActor<AABStageGimmick>(NewLocation, FRotator::ZeroRotator);
	}

}

void AABStageGimmick::OpenAllGates()
{
	for (const auto& Gate : Gates)
	{
		Gate.Value->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	}
}

void AABStageGimmick::CloseAllGates()
{
	for (const auto& Gate : Gates)
	{
		Gate.Value->SetRelativeRotation(FRotator::ZeroRotator);
		/*Gate.Value->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));*/
	}
}


void AABStageGimmick::SetReady()
{
	// 가운데 트리거 활성화
	StageTrigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);

	// 문 콜리전 비활성화
	for (const auto& GateTrigger : GateTriggers)
	{
		GateTrigger->SetCollisionProfileName(CPROFILE_NOCOLLISION);
	}

	// 문 열기
	OpenAllGates();

	UE_LOG(LogTemp, Log, TEXT("SetReady"));
}

void AABStageGimmick::SetFight()
{
	// 가운데 트리거 비활성화
	StageTrigger->SetCollisionProfileName(CPROFILE_NOCOLLISION);

	// 문 콜리전 비활성화
	for (const auto& GateTrigger : GateTriggers)
	{
		GateTrigger->SetCollisionProfileName(CPROFILE_NOCOLLISION);
	}

	// 문 닫기
	CloseAllGates();

	UE_LOG(LogTemp, Log, TEXT("SetFight"));

	// NPC 생성.
	GetWorld()->GetTimerManager().SetTimer(OpponentTimerHandle, this,
		&AABStageGimmick::OnOpponentSpawn,
		OpponentSpawnTime, false);
}

void AABStageGimmick::ChooseReward()
{
	// 가운데 트리거 비활성화
	StageTrigger->SetCollisionProfileName(CPROFILE_NOCOLLISION);

	// 문 콜리전 비활성화
	for (const auto& GateTrigger : GateTriggers)
	{
		GateTrigger->SetCollisionProfileName(CPROFILE_NOCOLLISION);
	}

	// 문 닫기
	CloseAllGates();

	UE_LOG(LogTemp, Log, TEXT("ChooseReward"));

	// 아이템 생성.
	SpawnRewardBoxes();
}

void AABStageGimmick::ChooseNext()
{
	// 가운데 트리거 비활성화
	StageTrigger->SetCollisionProfileName(CPROFILE_NOCOLLISION);

	// 문 콜리전 비활성화
	for (const auto& GateTrigger : GateTriggers)
	{
		GateTrigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);
	}

	// 문 닫기
	OpenAllGates();

	UE_LOG(LogTemp, Log, TEXT("ChooseNext"));
}

void AABStageGimmick::OnOpponentSpawn()
{
	// 생성 위치
	const FVector SpawnLocation = GetActorLocation() + FVector::UpVector * 88.0f;

	// 생성된 액터의 포인터
	AActor* OpponentActor = GetWorld()->SpawnActor(OpponentClass,& SpawnLocation, &FRotator::ZeroRotator);

	// NPC가 죽었을 때 발생하는 델리게이트에 함수 등록
	OpponentActor->OnDestroyed.AddDynamic(this, &AABStageGimmick::OnOppoenentDestroyed);
}

void AABStageGimmick::OnOppoenentDestroyed(AActor* DestroyedActor)
{
	// NPC가 죽으면 보상 단계로 전환
	SetState(EStageState::Reward);
}

void AABStageGimmick::OnRewardTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 캐릭터가 아이템 상자와 부딪힌 후의 처리.
	for (const auto& RewardBox : RewardBoxes)
	{
		// 약참조 중인 포인터가 유효한지 확인.
		if (RewardBox.IsValid())
		{
			// 순회 중인 아이템 상자가 캐릭터와 부딪힌 상자인지 비교.
			AABItemBox* ValidItemBox = RewardBox.Get();
			// 캐릭터와 부딪힌 액터
			AActor* OverlappedBox = OverlappedComponent->GetOwner();

			// 순회중인 박스가 캐릭터가 부딪힌 박스가 아니면, 직접 제거.
			if (ValidItemBox != OverlappedBox)
			{
				ValidItemBox->Destroy();
			}
		}
	}

	SetState(EStageState::Next);
}

void AABStageGimmick::SpawnRewardBoxes()
{
	for (const auto& RewardBoxLocation : RewardBoxLocations)
	{
		// 현재 스테이지 액터의 위치를 기준으로 아이템 상자 생성 위치 구하기.
		FVector SpawnLocation = GetActorLocation() + RewardBoxLocation.Value
			+ FVector(0.0f, 0.0f, 30.0f);

		AActor* ItemActor = GetWorld()->SpawnActor(RewardBoxClass, &SpawnLocation, &FRotator::ZeroRotator);

		// 아이템 박스가 가진 콜리전과 오버랩 이벤트 등록.
		AABItemBox* RewardBoxActor= Cast<AABItemBox>(ItemActor);
		if (RewardBoxActor)
		{
			RewardBoxActor->GetTrigger()->OnComponentBeginOverlap.AddDynamic(this, &AABStageGimmick::OnRewardTriggerBeginOverlap);

			// 생성된 아이템 상자를 배열에 추가.
			RewardBoxes.Add(RewardBoxActor);
		}
	}
}
