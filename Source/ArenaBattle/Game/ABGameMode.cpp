// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ABGameMode.h"
#include "Player/ABPlayerController.h"

AABGameMode::AABGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassRef(TEXT("/Script/Engine.Blueprint'/Game/ArenaBattle/Blueprint/BP_ABCharacterPlayer.BP_ABCharacterPlayer_C'"));
	if (DefaultPawnClassRef.Class)
	{
		DefaultPawnClass = DefaultPawnClassRef.Class;
	}

	static ConstructorHelpers::FClassFinder < APlayerController> PlayerControllerRef (TEXT("/Script/ArenaBattle.ABPlayerController"));
	if (PlayerControllerRef.Class)
	{
		PlayerControllerClass = PlayerControllerRef.Class;
	}

	ClearScore = 3;
	CurrentScore = 0;
	bIsCleared = false;
}

void AABGameMode::SetPlayerScore(int32 NewPlayerScore)
{
	CurrentScore = NewPlayerScore;

	OnScoreChanged.BroadCast(CurrentScore);

	//Get Current PlayerController - in single game
	AABPlayerController* ABPlayerController = Cast<AABPlayerController>(GetWorld()->GetFirstPlayerController());
	if (ABPlayerController)
	{
		//Request to ABPlayerController 
		ABPlayerController->GameScoreChanged(NewPlayerScore);
	}

	DetermineGameClear(ABPlayerController);
}

void AABGameMode::OnPlayerDead()
{
	check(bIsCleared == false);
	OnGameCleared.Broadcast(bIsCleared);

	//Get Current PlayerController - in single game
	AABPlayerController* ABPlayerController = Cast<AABPlayerController>(GetWorld()->GetFirstPlayerController());
	if (ABPlayerController)
	{
		//Request to ABPlayerController 
		ABPlayerController->GameOver();
	}
}

bool AABGameMode::IsGameCleared()
{
	return bIsCleared;
}

void AABGameMode::DetermineGameClear(AABPlayerController* InPlayerController)
{
	if (CurrentScore >= ClearScore)
	{
		bIsCleared = true;
		OnGameCleared.Broadcast(bIsCleared);

		if (InPlayerController)
		{
			//Request to ABPlayerController
			InPlayerController->GameClear();
		}
	}
}
