// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ABItemBox.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Physics/ABCollision.h"
#include "Interface/ABCharacterItemInterface.h"	// TODO->

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
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AABItemBox::OnOverlapBegin);

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
