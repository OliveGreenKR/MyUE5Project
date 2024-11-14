// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ABCharacterBase.h"
#include "Engine/StreamableManager.h"
#include "Interface/ABCharacterAIInterface.h"
#include "ABCharacterNonPlayer.generated.h"

/**
 * 
 */
UCLASS(config=ArenaBattle)
class ARENABATTLE_API AABCharacterNonPlayer : public AABCharacterBase , 
	public IABCharacterAIInterface
{
	GENERATED_BODY()
	
public:
	AABCharacterNonPlayer();

protected:
	virtual void PostInitializeComponents() override;
protected:
	void SetDead() override;
	void NPCMeshLoadCompleted();

	UPROPERTY(config)
	TArray<FSoftObjectPath> NPCMeshes;

	//Load with Asynchronous 
	TSharedPtr<FStreamableHandle> NPCMeshHandle;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Debug)
	uint8 bIsDummy : 1 = 0;

#pragma region AI
public:
	FAICharacterAttackFinished OnAttackFinished;
protected:
	// Inherited via IABCharacterAIInterface
	virtual float GetAIPatrolRadius() override;
	virtual float GetAIDetectRange() override;
	virtual float GetAIAttackRange() override;
	virtual float GetAITurnSpeed() override;

	/*void SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished) override;*/
	void AttackByAI() override;
	void StopAttackByAI() override;
private:
	bool GetAIDrawDebug() override { return bDrawDebug; }
#pragma endregion
#pragma region HitReaction
	//virtual void PlayHitReaction(float InBlendInTime = 1.0f) override;
#pragma endregion
};
