// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ABHpBarWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Interface/ABCharacterWidgetInterface.h"
#include "GameData/ABCharacterStat.h"

UABHpBarWidget::UABHpBarWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UABHpBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HpProgressBar = Cast<UProgressBar>(GetWidgetFromName(HpBarName));
	ensure(HpProgressBar);

	HpStatText = Cast<UTextBlock>(GetWidgetFromName(HpTextName));
	ensure(HpStatText);

	//Data Binding to OwningActor
	IABCharacterWidgetInterface* CharacterWidget = Cast<IABCharacterWidgetInterface>(OwningActor);
	if (CharacterWidget)
	{
		CharacterWidget->SetupCharacterWidget(this);
	}
}

void UABHpBarWidget::UpdateCurrentHP(float NewCurrentHp)
{ 
	float NewCurrent = FMath::Max(NewCurrentHp, 0);
	SetCurrentHp(NewCurrent);
}

void UABHpBarWidget::UpdateMaxHp(const FABCharacterStat& BaseStat, const FABCharacterStat& ModifierStat)
{
;	float NewMax = FMath::Max((BaseStat + ModifierStat).MaxHp, KINDA_SMALL_NUMBER);
	SetMaxtHp(NewMax);
}

void UABHpBarWidget::UpdateHpBar()
{
	ensure(MaxHp > 0.0f);

	if (!HpProgressBar)
	{
		return;
	}

	float NewHpPercent = FMath::Clamp(CurrentHp / MaxHp, 0.0f, 1.0f);

	HpProgressBar->SetPercent(NewHpPercent);
	HpStatText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), (int)CurrentHp, (int)MaxHp)));
}


