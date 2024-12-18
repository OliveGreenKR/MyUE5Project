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

void AABPlayerController::PostInitializeComponents()
{
	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("Begin"));
	Super::PostInitializeComponents();
	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("End"));
}

void AABPlayerController::PostNetInit()
{
	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("Begin"));
	Super::PostNetInit();

	UNetDriver* NetDriver = GetNetDriver();
	if (NetDriver)
	{
		if (NetDriver->ServerConnection)
		{
			AB_LOG(LogABNetwork, Log, TEXT("Server Connection : %s"), *NetDriver->ServerConnection->GetName());
		}
	}
	else
	{
		AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("No Net Driver"));
	}

	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("End"));
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