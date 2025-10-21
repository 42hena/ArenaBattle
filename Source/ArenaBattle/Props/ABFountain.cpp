// Fill out your copyright notice in the Description page of Project Settings.


#include "Props/ABFountain.h"

#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"

// Sets default values
AABFountain::AABFountain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 컴포넌트 생성.
	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	Water = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Water"));
	Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light"));

	// 루트 컴포넌트 설정.
	RootComponent = Body;

	// Water 컴포넌트는 Body에 붙어야 함.
	Water->SetupAttachment(Body, NAME_None);

	// 적절한 위치로 지정.
	Water->SetRelativeLocation(FVector(0, 0, 140.f));

	// 라이트 컴포넌트 계층 설정
	Light->SetupAttachment(Body);
	// 컴포넌트 위치 설정
	Light->SetRelativeLocation(FVector(0, 0, 200.0f));
	// 점조명이 영향을 미치는 반지름.
	Light->AttenuationRadius = 250.0f;

	// Body 에셋
	static ConstructorHelpers::FObjectFinder<UStaticMesh> FountainBodyMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/ArenaBattle/Environment/Props/SM_Plains_Castle_Fountain_01.SM_Plains_Castle_Fountain_01'"));
	if (FountainBodyMeshRef.Succeeded())
	{
		Body->SetStaticMesh(FountainBodyMeshRef.Object);
	}

	// Water 에셋
	static ConstructorHelpers::FObjectFinder<UStaticMesh> FountainWaterMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/ArenaBattle/Environment/Props/SM_Plains_Fountain_02.SM_Plains_Fountain_02'"));
	if (FountainWaterMeshRef.Succeeded())
	{
		Water->SetStaticMesh(FountainWaterMeshRef.Object);
	}
}

// Called when the game starts or when spawned
void AABFountain::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AABFountain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

