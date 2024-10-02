// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ABCharacterStat.h"
#include "ABGameSingleton.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogABGameSingleton, Error, All);

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABGameSingleton : public UObject
{
	GENERATED_BODY()
	
public:
	UABGameSingleton();
	static UABGameSingleton& Get();

#pragma region Character Stat Data
public:
	FORCEINLINE FABCharacterStat GetCharacterStat(int32 InLevel) const 
	{ 
		return CharacterStatTable.IsValidIndex(InLevel) 
			? CharacterStatTable[InLevel] 
			: FABCharacterStat();
	}
	
	UPROPERTY()
	int32 CharacterMaxLevel;

private:
	TArray<FABCharacterStat> CharacterStatTable;
#pragma endregion
};
