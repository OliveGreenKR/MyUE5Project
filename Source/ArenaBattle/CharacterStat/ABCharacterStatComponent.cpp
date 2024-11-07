// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStat/ABCharacterStatComponent.h"
#include "GameData/ABGameSingleton.h"

// Sets default values for this component's properties
UABCharacterStatComponent::UABCharacterStatComponent()
{
	CurrentLevel = 1;
	bWantsInitializeComponent = true;
}

void UABCharacterStatComponent::SetLevelStat(int32 InNewLevel)
{
	CurrentLevel = FMath::Clamp(InNewLevel, 1, UABGameSingleton::Get().CharacterMaxLevel);
	SetBaseStat(UABGameSingleton::Get().GetCharacterStat(CurrentLevel));
	check(BaseStat.MaxHp > 0)
}


void UABCharacterStatComponent::AddBaseStat(const FABCharacterStat& InDeltaStat)
{
	SetBaseStat(BaseStat + InDeltaStat);
	AddCurrentHp(InDeltaStat.MaxHp);
}

float UABCharacterStatComponent::ApplyDamage(float InDamage)
{
	// get Always 0 or higher Damage value.
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0, InDamage);

	AddCurrentHp(-ActualDamage);

	if (CurrentHp <= KINDA_SMALL_NUMBER)
	{
		//Dead
		OnHpZero.Broadcast();
	}

	return ActualDamage;
}

void UABCharacterStatComponent::SetCurrentHp(float NewHp)
{
	CurrentHp = FMath::Clamp<float>(NewHp, 0.0f, GetTotalStat().MaxHp);
	OnHpChanged.Broadcast(CurrentHp);
}

void UABCharacterStatComponent::InitializeComponent()
{
	Super::InitializeComponent();

	SetLevelStat(CurrentLevel);
	SetCurrentHp(GetTotalStat().MaxHp);
}

