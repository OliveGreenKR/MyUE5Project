// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ABSkillExecutorInterface.generated.h"

DECLARE_DELEGATE(FOnCharacterSkillEndDelegate);
DECLARE_DELEGATE(FOnCharacterSkillBeginDelegate);

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UABSkillExecutorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ARENABATTLE_API IABSkillExecutorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	struct SkillParameters
	{
		float SkillSpeedRate = 1.0f;
		float SkillRangeForwardModifier = 0.0f;
		FVector3f SkillExtentRate = FVector3f::OneVector;
		float SkillDamageModifier = 0.0f;
		float SkillDamageMultiplier = 1.0f;
	};
public:
	virtual void ExecuteSkill(const SkillParameters& InSkillParams, FVector DesiredDiretion, bool DrawDebug = false) = 0;
	virtual void CancelSkill() = 0;
	virtual void ForcelySetSkillDirection(FVector InDesiredDirection) = 0;
	virtual const float GetSkillRange() const = 0;

	FOnCharacterSkillEndDelegate OnSkillEnd;
	FOnCharacterSkillBeginDelegate OnSkillBegin;
};
