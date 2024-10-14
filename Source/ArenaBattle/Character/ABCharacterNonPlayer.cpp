#include "Character/ABCharacterNonPlayer.h"
#include "Engine/AssetManager.h"
#include "CharacterStat/ABCharacterStatComponent.h"
#include "CharacterSkill/ABCharacterSkillComponent.h"
#include "AI/ABAIController.h"

AABCharacterNonPlayer::AABCharacterNonPlayer()
{
	GetMesh()->SetHiddenInGame(true);

	AIControllerClass = AABAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

}

void AABCharacterNonPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ensure(NPCMeshes.Num() > 0);
	int32 RandIndex = FMath::RandRange(0, NPCMeshes.Num() - 1);
	NPCMeshHandle = UAssetManager::Get().GetStreamableManager().
		RequestAsyncLoad(NPCMeshes[RandIndex], FStreamableDelegate::CreateUObject(this, &AABCharacterNonPlayer::NPCMeshLoadCompleted));

	if (bIsDummy)
	{
		AIControllerClass = nullptr;
		bDrawDebug = false;
	}
	else
	{
		bDrawDebug = true;
	}
}

void AABCharacterNonPlayer::SetDead()
{
	if (bIsDummy)
		return;

	Super::SetDead();
	
	FTimerHandle DeadTimerHandle;

	GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda(
		[&]() {
			Destroy();
		}),
		DeadEventDelayTime, false);
}

void AABCharacterNonPlayer::NPCMeshLoadCompleted()
{
	if (NPCMeshHandle.IsValid())
	{
		USkeletalMesh* NPCMesh = Cast<USkeletalMesh>(NPCMeshHandle->GetLoadedAsset());
		if (NPCMesh)
		{
			GetMesh()->SetSkeletalMesh(NPCMesh);
			GetMesh()->SetHiddenInGame(false);
		}
	}

	NPCMeshHandle->ReleaseHandle();
}

float AABCharacterNonPlayer::GetAIPatrolRadius()
{
	return 800.0f;
}

float AABCharacterNonPlayer::GetAIDetectRange()
{
	return 450.0f;
}

float AABCharacterNonPlayer::GetAIAttackRange()
{
	float ret = 0.0f;
	if (BasicSkillComponent)
	{
		ret = BasicSkillComponent->GetSkillRange();
	}
	return ret;
}

float AABCharacterNonPlayer::GetAITurnSpeed()
{
	return 0.0f;
}

void AABCharacterNonPlayer::AttackByAI()
{
	if (BasicSkillComponent)
	{
		using SkillParameters = IABSkillExecutorInterface::SkillParameters;

		SkillParameters OutSkillParams = SkillParameters();
		FABCharacterStat TotalStat = Stat->GetTotalStat();

		OutSkillParams.SkillDamageModifier =  bDrawDebug ? 0.0f : TotalStat.Attack;

		OutSkillParams.SkillExtentRate = FVector3f(TotalStat.AttackRangeRate);
		OutSkillParams.SkillRangeForwardModifier = TotalStat.AttackRangeForward;
		OutSkillParams.SkillSpeedRate = bDrawDebug ? 0.85f : TotalStat.AttackSpeedRate;
		
		BasicSkillComponent->ExecuteSkill(OutSkillParams, GetLastMovementInputVector(), bDrawDebug);
	}
}

