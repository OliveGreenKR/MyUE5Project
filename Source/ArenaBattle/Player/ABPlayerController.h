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
#pragma region SaveGame
protected:
	UFUNCTION(BlueprintImplementableEvent, Category = Game, Meta = (DisplayName = "OnGameRetryCountCpp"))
	void K2_OnGameRetryCount(int32 NewRetryCount);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveGame)
	TObjectPtr<class UABSaveGame> SaveGameInstance;
#pragma endregion


//#pragma region HUD
//protected:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = HUD)
//	TSubclassOf<class UABHUDWidget> ABHUDWidgetClass;
//
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
//	TObjectPtr<class UABHUDWidget> ABHUDWidget;
//#pragma endregion
};
