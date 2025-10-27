// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ABWidgetComponent.h"

#include "ABUserWidget.h"

void UABWidgetComponent::InitWidget()
{
	Super::InitWidget();

	// 상위에서 InitWidget 함수가 호출된 이후에는
	// 위젯 인스턴스가 

	UABUserWidget* ABUserWidget = Cast<UABUserWidget>(GetWidget());
	if (ABUserWidget)
	{
		ABUserWidget->SetOwningActor(GetOwner());
	}
}
