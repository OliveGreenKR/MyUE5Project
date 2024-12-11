// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ABPlayerController.h"
#include "UI/ABHUDWidget.h"
#include "Interface/ABGameInterface.h"
#include "Player/ABSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "ArenaBattle.h"

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
	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("Begin"));

	Super::BeginPlay();

	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("End"));

	//Respond to Player Input
	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}