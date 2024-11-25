// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ABPlayerController.h"
#include "UI/ABHUDWidget.h"
#include "Interface/ABGameInterface.h"
#include "Player/ABSaveGame.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogABPlayerController);

AABPlayerController::AABPlayerController()
{
	//static ConstructorHelpers::FClassFinder<UABHUDWidget> ABHUDWidgetRef(TEXT("/Game/ArenaBattle/UI/WBP_ABHUD.WBP_ABHUD_C"));
	//if (ABHUDWidgetRef.Class)
	//{
	//	ABHUDWidgetClass = ABHUDWidgetRef.Class;
	//}
}

void AABPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//Respond to Player Input
	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);

	//Load GameSave
	SaveGameInstance = Cast<UABSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("Player0"), 0)); // '0' for single play UserIndex
	if (!SaveGameInstance)
	{
		SaveGameInstance = NewObject<UABSaveGame>();
		SaveGameInstance->RetryCount = 0;
	}
	else
	{
		SaveGameInstance->RetryCount++;
	}

	K2_OnGameRetryCount(SaveGameInstance->RetryCount);
}

void AABPlayerController::GameScoreChanged(int32 NewScore)
{
	K2_OnScoreChanged(NewScore);
}

void AABPlayerController::GameOver()
{
	K2_OnGameOver();

	if (!UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("Player0"), 0))
	{
		UE_LOG(LogABPlayerController, Error, TEXT("Save Game Error!"));
	}

	K2_OnGameRetryCount(SaveGameInstance->RetryCount);
}

void AABPlayerController::GameClear()
{
	K2_OnGameClear();

	SaveGameInstance->RetryCount = 0;

	if (!UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("Player0"), 0))
	{
		UE_LOG(LogABPlayerController, Error, TEXT("Save Game Error!"));
	}

	K2_OnGameRetryCount(SaveGameInstance->RetryCount);
}


