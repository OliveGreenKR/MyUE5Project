#include "Character/ABCharacterNonPlayer.h"

ABCharacterNonPlayer::ABCharacterNonPlayer()
{
}

void ABCharacterNonPlayer::SetDead()
{
	Super::SetDead();
	
	FTimerHandle DeadTimerHandle;

	GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda(
		[&]() {
			Destroy();
		}),
		DeadEventDelayTime, false);
}

