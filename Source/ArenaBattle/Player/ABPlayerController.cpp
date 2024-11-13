// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ABPlayerController.h"
#include "UI/ABHUDWidget.h"
#include "Interface/ABGameInterface.h"


AABPlayerController::AABPlayerController()
{
	//static ConstructorHelpers::FClassFinder<UABHUDWidget> ABHUDWidgetRef(TEXT("/Game/ArenaBattle/UI/WBP_ABHUD.WBP_ABHUD_C"));
	//if (ABHUDWidgetRef.Class)
	//{
	//	ABHUDWidgetClass = ABHUDWidgetRef.Class;
	//}
}

void AABPlayerController::GameScoreChanged(int32 NewScore)
{
	K2_OnScoreChanged(NewScore);
}

void AABPlayerController::GameOver()
{
	K2_OnGameOver();
}

void AABPlayerController::GameClear()
{
	K2_OnGameClear();
}

void AABPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//Respond to Player Input
	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}
