// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ABHUDWidget.h"
#include "Interface/ABCharacterHUDInterface.h"
#include "ABHpBarWidget.h"
#include "ABCharacterStatWidget.h"
#include "GameData/ABCharacterStat.h"

UABHUDWidget::UABHUDWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UABHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HpBar = Cast<UABHpBarWidget>(GetWidgetFromName(TEXT("WidgetHpBar")));
	ensure(HpBar);

	CharacterStatDisplay = Cast<UABCharacterStatWidget>(GetWidgetFromName(TEXT("WidgetCharacterStat")));
	ensure(CharacterStatDisplay);

	IABCharacterHUDInterface* HUDPawn = Cast<IABCharacterHUDInterface>(GetOwningPlayerPawn());
	if (HUDPawn)
	{
		HUDPawn->SetupHUDWidget(this);
	}
}

void UABHUDWidget::UpdateStat(const FABCharacterStat& BaseStat, const FABCharacterStat& ModifierStat)
{
	HpBar->UpdateMaxHp(BaseStat, ModifierStat);
	CharacterStatDisplay->UpdateStat(BaseStat, ModifierStat);
}

void UABHUDWidget::UpdateHpBar(float NewCurrentHp)
{
	HpBar->UpdateCurrentHP(NewCurrentHp);
}

