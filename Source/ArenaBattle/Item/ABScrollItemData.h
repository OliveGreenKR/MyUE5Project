// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ABItemData.h"
#include "GameData/ABCharacterStat.h"
#include "ABScrollItemData.generated.h"

/**
 * Item for increasing Character's Base Stat
 */
UCLASS()
class ARENABATTLE_API UABScrollItemData : public UABItemData
{
	GENERATED_BODY()
public:
	UABScrollItemData();

public:
	UPROPERTY(EditAnywhere, Category = Stat)
	FABCharacterStat BaseStat;
};
