// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterNonPlayer.h"

#include "Engine/AssetManager.h"
#include "AI/ABAIController.h"

AABCharacterNonPlayer::AABCharacterNonPlayer()
{
	// 시작할 때는 메시가 안보이도록 설정.
	GetMesh()->SetHiddenInGame(true);

	AIControllerClass = AABAIController::StaticClass();

	// 맵에서 배치하거나 동적으로 생성될 때 모두 AIController에 빙의되도록 설정.
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

}

void AABCharacterNonPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 확인
	ensureAlways(NPCMeshes.Num() > 0);
	
	// 랜덤으로 배열 인덱스 선택
	int32 RandomIndex = FMath::RandRange(0, NPCMeshes.Num() - 1);

	// 비동기 로딩
	NPCMeshHandle = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(NPCMeshes[RandomIndex],
		FStreamableDelegate::CreateUObject(this, &AABCharacterNonPlayer::NPCMeshLoadCompleted));
}

void AABCharacterNonPlayer::SetDead()
{
	Super::SetDead();

	// 타이머를 사용해서 일정 시간 이후에 삭제 처리
	FTimerHandle DeadTimerHandle;
	FTimerManagerTimerParameters params;

	auto Delegate = FTimerDelegate::CreateLambda(
		[&]()
		{
			Destroy();
		}
	);
	// 타이머를 사용해서 일정 시간 이우헤 삭제 처리.
	GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, Delegate, DeadEventDelayTime, false);
}

void AABCharacterNonPlayer::NPCMeshLoadCompleted()
{
	// 에셋 로드가 끝났을 때 핸들이 유호한지 확인
	if (NPCMeshHandle.IsValid())
	{
		// UObject에서 다운캐스팅
		USkeletalMesh* NPCMesh = Cast<USkeletalMesh>(NPCMeshHandle->GetLoadedAsset());
		if (NPCMesh)
		{
			// 스켈리탈 메시 설정
			GetMesh()->SetSkeletalMesh(NPCMesh);

			// 감춰놨던 메시 컴포넌트를 다시 보이게 설정.
			GetMesh()->SetHiddenInGame(false);
		}
	}
	// 모든 작업이 끝난 후 핸들 해제
	NPCMeshHandle->ReleaseHandle();
}
