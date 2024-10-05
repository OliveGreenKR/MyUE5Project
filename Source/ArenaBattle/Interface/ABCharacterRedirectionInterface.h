// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ABCharacterRedirectionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UABCharacterRedirectionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ARENABATTLE_API IABCharacterRedirectionInterface
{
	GENERATED_BODY()
public:

	enum class ERedirectionPrioirty : uint8
	{
		None = 255,
		Skill = 1,
	};

	virtual void CharacterRedireciton(const FVector& InDirection, const ERedirectionPrioirty prioirty) = 0;
};