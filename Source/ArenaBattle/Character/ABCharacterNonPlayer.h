// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ABCharacterBase.h"
#include "ABCharacterNonPlayer.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API ABCharacterNonPlayer : public AABCharacterBase
{
	GENERATED_BODY()
	
public:
	ABCharacterNonPlayer();

protected:
	void SetDead() override;
};
