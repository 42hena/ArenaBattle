// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterNonPlayer.h"

AABCharacterNonPlayer::AABCharacterNonPlayer()
{

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
