// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ABPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AABPlayerController();

#pragma region GameMode
protected:
	//For BP_Node 
	UFUNCTION(BlueprintImplementableEvent, Category = Game,Meta = (DIsplayName = "OnScoreChangedCPP"))
	void K2_OnScoreChanged(int32 NewScore);
	UFUNCTION(BlueprintImplementableEvent, Category = Game, Meta = (DIsplayName = "OnGameOverCPP"))
	void K2_OnGameOver();
	UFUNCTION(BlueprintImplementableEvent, Category = Game, Meta = (DIsplayName = "OnGameClearCPP"))
	void K2_OnGameClear();

public :
	void GameScoreChanged(int32 NewScore);
	void GameOver();
	void GameClear();
#pragma endregion
	
protected:
	virtual void BeginPlay() override;


#pragma region HUD
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = HUD)
	TSubclassOf<class UABHUDWidget> ABHUDWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	TObjectPtr<class UABHUDWidget> ABHUDWidget;
#pragma endregion
};
