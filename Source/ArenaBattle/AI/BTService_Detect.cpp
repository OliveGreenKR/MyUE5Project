// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_Detect.h"

#include "ABAI.h"
#include "AIController.h"
#include "Interface/ABCharacterAIInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Physics/ABCollision.h"
#include "DrawDebugHelpers.h"
#include "Engine/OverlapResult.h"

UBTService_Detect::UBTService_Detect()
{
	NodeName = TEXT("Detect");
	Interval = 0.5f;
}

void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn)
	{
		return;
	}

	UWorld* World = ControllingPawn->GetWorld();
	if (nullptr == World)
	{
		return;
	}

	IABCharacterAIInterface* AIPawn = Cast<IABCharacterAIInterface>(ControllingPawn);
	if (nullptr == AIPawn)
	{
		return;
	}

	float DetectRadius = AIPawn->GetAIDetectRange();
	bool DrawDebug = AIPawn->GetAIDrawDebug();
	FVector Center = ControllingPawn->GetActorLocation();
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParam(SCENE_QUERY_STAT(Detect), false, ControllingPawn);
	bool bResult = World->OverlapMultiByChannel(
		OverlapResults,
		Center,
		FQuat::Identity,
		CCHANNEL_ABACTION,
		FCollisionShape::MakeSphere(DetectRadius),
		CollisionQueryParam
	);
	if (DrawDebug)
	{
		DrawDebugSphere(GetWorld(), Center, DetectRadius, 32, FColor::Emerald, false, Interval);
	}

	if (OverlapResults.Num() > 5)
	{
		OverlapResults.SetNum(5);
	}

	//Align HitResults
	OverlapResults.Sort([Center](const FOverlapResult& A, const FOverlapResult& B) {
		return FVector::DistSquared(Center, A.GetActor()->GetActorLocation()) < FVector::DistSquared(Center, B.GetActor()->GetActorLocation());
						});

	//Save DetectionResult to BB's Target
	if (bResult)
	{
		for (auto const& OverlapResult : OverlapResults)
		{
			APawn* Pawn = Cast<APawn>(OverlapResult.GetActor());
			if (Pawn && Pawn->GetController()->IsPlayerController())
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(BBKEY_TARGET, Pawn);
				if (DrawDebug)
				{
					DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Green, false, Interval * 0.5f);

					DrawDebugPoint(World, Pawn->GetActorLocation(), 10.0f, FColor::Green, false, Interval * 0.5f);
					DrawDebugLine(World, ControllingPawn->GetActorLocation(), Pawn->GetActorLocation(), FColor::Green, false, Interval * 0.55f);
				}
				return;
			}
		}
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(BBKEY_TARGET, nullptr);
		if (DrawDebug)
		{
			DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Red, false, Interval * 0.5f);
		}
	}
}


