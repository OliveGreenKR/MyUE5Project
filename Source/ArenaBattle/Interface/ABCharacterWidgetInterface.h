// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ABCharacterWidgetInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UABCharacterWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * An interface that performs Character attaching Widget to itself.
 */
class ARENABATTLE_API IABCharacterWidgetInterface
{
	GENERATED_BODY()

public:
	virtual void SetupCharacterWidget(class UABUserWidget* InUserWidget) = 0;
};
