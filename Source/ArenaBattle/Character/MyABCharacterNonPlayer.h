// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ABCharacterBase.h"
#include "MyABCharacterNonPlayer.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AMyABCharacterNonPlayer : public AABCharacterBase
{
	GENERATED_BODY()
	
public:
	AMyABCharacterNonPlayer();

protected:
	void SetDead() override;
};
