// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ABPlayerController.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogABPlayerController, Log, All);

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AABPlayerController();

protected:
	virtual void BeginPlay() override;

//#pragma region HUD
//protected:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = HUD)
//	TSubclassOf<class UABHUDWidget> ABHUDWidgetClass;
//
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
//	TObjectPtr<class UABHUDWidget> ABHUDWidget;
//#pragma endregion
};
