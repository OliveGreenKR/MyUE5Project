// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_StopAttack.h"
#include "AIController.h"
#include "Interface/ABCharacterAIInterface.h"

UBTTask_StopAttack::UBTTask_StopAttack()
{
}

EBTNodeResult::Type UBTTask_StopAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);


	APawn* ControllingPawn = Cast<APawn>(OwnerComp.GetAIOwner()->GetPawn());
	if (!ControllingPawn->IsValidLowLevelFast())
	{
		return EBTNodeResult::Failed;
	}

	IABCharacterAIInterface* AIPawn = Cast<IABCharacterAIInterface>(ControllingPawn);
	if (nullptr == AIPawn)
	{
		return EBTNodeResult::Failed;
	}

	AIPawn->StopAttackByAI();
	return EBTNodeResult::Succeeded;

	return Result;
}
