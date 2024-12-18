// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ABAIController.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AABAIController();

public:
	void RunAI();
	void StopAI();

protected:
	virtual void OnPossess(APawn* InPawn) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UBlackboardData> BBAsset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UBehaviorTree> BTAsset;

};
