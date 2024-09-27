// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ABAnimationAttackInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UABAnimationAttackInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * An interface that performs hit detection at the requested time
 */
class ARENABATTLE_API IABAnimationAttackInterface
{
	GENERATED_BODY()

public:
	virtual void PerformSkillHitCheck() = 0;
};
