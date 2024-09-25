// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MyABCharacterNonPlayer.h"

AMyABCharacterNonPlayer::AMyABCharacterNonPlayer()
{
}

void AMyABCharacterNonPlayer::SetDead()
{
	Super::SetDead();
	
	FTimerHandle DeadTimerHandle;

	GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda(
		[&]() {
			Destroy();
		}),
		DeadEventDelayTime, false);
}

