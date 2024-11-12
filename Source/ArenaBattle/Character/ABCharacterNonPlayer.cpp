#include "Character/ABCharacterNonPlayer.h"
#include "Engine/AssetManager.h"
#include "CharacterStat/ABCharacterStatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CharacterSkill/ABCharacterSkillComponent.h"
#include "AI/ABAIController.h"
#include "NavigationSystem.h"

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

	BasicSkillComponent->Activate();

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

	if (AABAIController* AIController = Cast<AABAIController>(Controller))
	{
		AIController->StopAI();
	}
	
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
	if (BasicSkillComponent)
	{
		return BasicSkillComponent->GetSkillRange();
	}
	return 0.0f;
}

float AABCharacterNonPlayer::GetAITurnSpeed()
{
	return 3.0f;
}

void AABCharacterNonPlayer::AttackByAI()
{
	if (BasicSkillComponent)
	{
		using SkillParameters = IABSkillExecutorInterface::SkillParameters;

		SkillParameters OutSkillParams = SkillParameters();
		FABCharacterStat TotalStat = Stat->GetTotalStat();

		OutSkillParams.SkillDamageModifier = TotalStat.Attack;

		OutSkillParams.SkillExtentRate = FVector3f(TotalStat.AttackRangeRate);
		OutSkillParams.SkillRangeForwardModifier = TotalStat.AttackRangeForward;
		OutSkillParams.SkillSpeedRate = TotalStat.AttackSpeedRate;

		BasicSkillComponent->OnSkillBegin.BindLambda([&]() { GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None); });
		BasicSkillComponent->OnSkillEnd.BindLambda([&]() { GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking); });

		BasicSkillComponent->ExecuteSkill(OutSkillParams, bDrawDebug);
	}
}

void AABCharacterNonPlayer::StopAttackByAI()
{
	if (BasicSkillComponent && BasicSkillComponent->IsActive())
	{
		BasicSkillComponent->CancelSkill();
	}
}

