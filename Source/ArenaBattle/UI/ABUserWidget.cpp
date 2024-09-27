// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ABUserWidget.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"

void UABUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// WidgetComponent�� ���� ������ Actor�� ������
	if (UWidgetComponent* WidgetComponent = Cast<UWidgetComponent>(GetOuter()))
	{
		OwningActor = WidgetComponent->GetOwner();
	}
}