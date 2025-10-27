// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ABItemBox.generated.h"

UCLASS()
class ARENABATTLE_API AABItemBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AABItemBox();

protected:
	// Box 컴포넌트의 OnComponentBeginOverlap 델리게이트에 등록할 함수.
	// UFUNCTION으로 지정해야 함.(Dynamic이라고 UFUNCTION 필수)
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 상자 획득 시 파티클 재생이 완료되면 실행할 함수.
	// OnSystemFinished 델리게이트에 등록
	UFUNCTION()
	void OnEffectFinished(class UParticleSystemComponent* PSystem);

protected:
	// 충돌을 위한 박스 컴포넌트 
	UPROPERTY(VisibleAnywhere, Category = Box)
	TObjectPtr<class UBoxComponent> Trigger;

	// 아이템 박스 메시를 보여주기 위한 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = Box)
	TObjectPtr<class UStaticMeshComponent> Mesh;

	// 아이템 습득 시 이펙트를 보여주는 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = Box)
	TObjectPtr<class UParticleSystemComponent> Effect;


	UPROPERTY(EditAnywhere, Category = Item)
	TObjectPtr<class UABItemData> Item;
};
