// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ABHpBarWidget.h"
#include "Components/ProgressBar.h"

UABHpBarWidget::UABHpBarWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

void UABHpBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HpProgressBar = Cast<UProgressBar>(GetWidgetFromName(HpBarName));
	ensure(HpProgressBar);

	//Data Binding to OwningActor
}

void UABHpBarWidget::UpdateHpBar(float NewCurrentHp)
{
	ensure(MaxHp > 0.0f);

	if (!HpProgressBar)
	{
		return;
	}

	float NewHpPercent = FMath::Clamp(NewCurrentHp / MaxHp, 0.0f, 1.0f);

	HpProgressBar->SetPercent(NewHpPercent);

}


