// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ABItemBox.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Physics/ABCollision.h"
#include "Interface/ABCharacterItemInterface.h"
#include "Engine/AssetManager.h"
#include "ABItemData.h"

// Sets default values
AABItemBox::AABItemBox()
{
	PrimaryActorTick.bCanEverTick = false;

	// 컴포넌트 생성
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Effect"));


	// 계층 설정
	RootComponent = Trigger;
	Mesh->SetupAttachment(Trigger);
	Effect->SetupAttachment(Trigger);

	// 콜리전 프로필 설정
	Trigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);

	// Todo: 콜리전 크기 설정
	Trigger->SetBoxExtent(FVector(42, 42, 30));
	
	
	// BeginOverlap 델리게이트에 함수 등록
	// Trigger->OnComponentBeginOverlap.AddDynamic(this, &AABItemBox::OnOverlapBegin);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BoxMeshRef(TEXT("/Game/ArenaBattle/Environment/Props/SM_Env_Breakables_Box1.SM_Env_Breakables_Box1"));
	if (BoxMeshRef.Object)
	{
		Mesh->SetStaticMesh(BoxMeshRef.Object);
	}

	Mesh->SetRelativeLocation(FVector(0, -3.5f, -30.0f));
	Mesh->SetCollisionProfileName(TEXT("NoCollision"));

	// Todo: 파티클 에셋 설정.
	static ConstructorHelpers::FObjectFinder<UParticleSystem> EffectRef(TEXT("/Game/ArenaBattle/Effect/P_TreasureChest_Open_Mesh.P_TreasureChest_Open_Mesh"));
	if (EffectRef.Object)
	{
		// EffectRef
		Effect->SetTemplate(EffectRef.Object);
		// 자동 재생 비활성화
		Effect->bAutoActivate = false;
	}

}

void AABItemBox::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 랜덤으로 아이템 데이터를 설정.
	UAssetManager& Manager = UAssetManager::Get();

	// 애샛 매니저에서 ABItemData 타입으로 애셋 목록 가져오기.
	TArray<FPrimaryAssetId> Results;
	Manager.GetPrimaryAssetIdList(TEXT("ABItemData"), Results);

	// 결과 확인(예외 처리).
	ensureAlways(Results.Num() > 0);

	// 랜덤 인덱스.
	int32 RandomIndex = FMath::RandRange(0, Results.Num() - 1);

	// 애셋 경로 가져오기.
	FSoftObjectPath AssetPath = Manager.GetPrimaryAssetPath(Results[RandomIndex]);

	// 로드를 위한 소프트 참조 생성.
	FSoftObjectPtr AssetToLoad(AssetPath);

	// 로드 처리
	if (AssetToLoad.IsPending())
	{
		AssetToLoad.LoadSynchronous();
	}

	// 로드한 애셋을 아이템에 설정.
	Item = Cast<UABItemData>(AssetToLoad.Get());

	// 제대로 설정됐는지 확인.
	ensureAlways(Item);
	
	// 오버랩 델리게이트에 함수 등록
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AABItemBox::OnOverlapBegin);
}

void AABItemBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!Item)
	{
		Destroy();
		return;
	}

	// 아이템이 있으면 캐릭터에 아이템 습득 메시지 전달.
	IABCharacterItemInterface* OverlappingPawn = Cast<IABCharacterItemInterface>(OtherActor);
	if (OverlappingPawn)
	{
		OverlappingPawn->TakeItem(Item);
	}


	// 아이템 습득 후 처리 진행.
	
	// 파티클 재생
	Effect->Activate(true);

	// 메시 비활성화
	Mesh->SetHiddenInGame(true);

	// 콜리전 비활성화(컴포넌트가 가지고 있는 모든애들 삭제)
	SetActorEnableCollision(false);

	// 파티클 재생 끝나면 발행되는 델리게이트에 등록.
	// 파티클 재생 종료 후에 아이템 액터 제거 목적
	Effect->OnSystemFinished.AddDynamic(this, &AABItemBox::OnEffectFinished);
}

void AABItemBox::OnEffectFinished(UParticleSystemComponent* PSystem)
{
	Destroy();
}
