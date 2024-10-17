// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSkill/ABCharacterSkillComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "Animation/AnimMontage.h"
#include "Character/ABComboActionData.h"
#include "SkillData/ABSkillData.h"
#include "Physics/ABCollision.h"
#include "Engine/DamageEvents.h"

// Sets default values for this component's properties
UABCharacterSkillComponent::UABCharacterSkillComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<UABSkillData> SkillDataRef(TEXT("/Script/ArenaBattle.ABSkillData'/Game/ArenaBattle/Skill/ABS_BasicSwordSkill.ABS_BasicSwordSkill'"));
	if (SkillDataRef.Object)
	{
		SkillData = SkillDataRef.Object;
	}
}

void UABCharacterSkillComponent::ExecuteSkill(const SkillParameters& InSkillParams, bool DrawDebug)
{
	ProcessSkill(InSkillParams, DrawDebug);
}

void UABCharacterSkillComponent::CancelSkill()
{
	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	
	AnimInstance->Montage_Stop(0.2f, SkillData->SkillMontage);

	GetWorld()->GetTimerManager().ClearTimer(ComboTimerHandle);
	bHasNextComboCommand = false;
	SkillEnd();
}

const float UABCharacterSkillComponent::GetSkillRange() const
{
	return OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius()+ GetNextSkillHitRange();
}

void UABCharacterSkillComponent::OnRegister()
{
	Super::OnRegister();
	OwnerCharacter = CastChecked<ACharacter>(GetOwner());
}

void UABCharacterSkillComponent::ProcessSkill(const SkillParameters& InSkillParams, bool DrawDebug)
{
	//First
	if (!IsCombo())
	{
		LastSkillParams = InSkillParams;
		bDrawDebug = DrawDebug;
		SkillBegin();
		return;
	}
	bHasNextComboCommand = ComboTimerHandle.IsValid();
}

void UABCharacterSkillComponent::SetComboNext()
{
	CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, SkillData->ComboActionData->MaxComboCount); 
}

void UABCharacterSkillComponent::SkillBegin()
{
	OnSkillBegin.ExecuteIfBound();

	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	ensureMsgf(AnimInstance, TEXT("%s doesn't have AnimInstance"), *(GetOwner()->GetName()));

	CurrentCombo = 1;
	
	UCharacterMovementComponent* Movement = OwnerCharacter->GetCharacterMovement();

	//TimerSet
	ComboTimerHandle.Invalidate();
	SetComboCheckTimer();

	//Play Anim
	AnimInstance->Montage_Play(SkillData->SkillMontage, GetCurrentSkillSpeedRate());

	//Set Montage End Deligate
	FOnMontageEnded EndDeligate;
	EndDeligate.BindUObject(this, &UABCharacterSkillComponent::OnSkillMontageEnd);
	AnimInstance->Montage_SetEndDelegate(EndDeligate, SkillData->SkillMontage);
}

void UABCharacterSkillComponent::OnSkillMontageEnd(UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
	SkillEnd();
}

void UABCharacterSkillComponent::SkillEnd()
{
	ensure(CurrentCombo != 0);
	ResetCombo();
	bDrawDebug = false;
	OnSkillEnd.ExecuteIfBound();
}

void UABCharacterSkillComponent::SetComboCheckTimer()
{
	OnSkillSeqBegin.Broadcast();
	int32 ComboIndex = GetCurrentCombo() - 1;
	UABComboActionData* ComboActionData = SkillData->ComboActionData;
	ensure(ComboActionData->EffectiveFrameCount.IsValidIndex(ComboIndex));
	float ComboEffectiveTime = (ComboActionData->EffectiveFrameCount[ComboIndex] / ComboActionData->FrameRate) * SkillData->SkillMontage->GetSectionLength(ComboIndex);
	ComboEffectiveTime = ComboEffectiveTime / GetCurrentSkillSpeedRate();

	if (ComboEffectiveTime < 0)
	{
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &UABCharacterSkillComponent::CheckSkillCombo, ComboEffectiveTime, false);
}

void UABCharacterSkillComponent::CheckSkillCombo()
{
	OnSkillSeqEnd.Broadcast();
	ComboTimerHandle.Invalidate();
	//Check Next Command
	if (bHasNextComboCommand)
	{
		UAnimInstance * AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
		//NextCombo Index
		SetComboNext();

		//Play Next Combo Animation
		FName NextSection = *FString::Printf(TEXT("%s%d"), *SkillData->ComboActionData->MontageSectionNamePrefix, GetCurrentCombo());
		AnimInstance->Montage_JumpToSection(NextSection, SkillData->SkillMontage);
		AnimInstance->Montage_SetPlayRate(SkillData->SkillMontage, GetCurrentSkillSpeedRate());
		SetComboCheckTimer();

		bHasNextComboCommand = false;
	}
}

void UABCharacterSkillComponent::PerformSkillHitCheck()
{
	TArray<FHitResult> OutHitResults;
	//tag :Attack
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, GetOwner());

	const FVector SkillDirection = GetOwner()->GetActorForwardVector();
	const FSkillDataPerMotion& NowMotionData = SkillData->GetSkillMotionData(GetCurrentCombo() - 1);

	const FQuat CollisionRotation = FRotationMatrix::MakeFromXZ(SkillDirection, FVector::UpVector).ToQuat(); //forward to unit:x
	const FVector Start = GetOwner()->GetActorLocation() +
		SkillDirection * OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FCollisionShape CollisionShape = GetCurrentSkillShape();
	const FVector End = GetOwner()->GetActorLocation() + SkillDirection * GetCurrentSkillHitRange();

	bool HitDetected = GetWorld()->SweepMultiByChannel(OutHitResults, Start, End, CollisionRotation, CCHANNEL_ABACTION, CollisionShape, Params);
	if (HitDetected)
	{

		FDamageEvent DamageEvent;
		ACharacter* Owner = Cast<ACharacter>(GetOwner());

		for (const FHitResult& HitResult : OutHitResults)
		{
			float NewDamage = (NowMotionData.SkillRawDamage + LastSkillParams.SkillDamageModifier) * LastSkillParams.SkillDamageMultiplier;
			HitResult.GetActor()->TakeDamage(NewDamage, DamageEvent, Owner->GetController(), Owner);
		}

	}

	if (bDrawDebug)
	{
		const FVector Origin = (Start + End) * 0.5f;
		const FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;
		const float DebugDuration = 1.0f;
		DrawDebugSkillCollision(Origin, CollisionRotation, DrawColor, DebugDuration);
	
		//Draw hit point
		for (const FHitResult& HitResult : OutHitResults)
		{
			DrawDebugBox(GetWorld(),HitResult.GetActor()->GetActorLocation(), FVector(20.0f), FColor::Blue, false, DebugDuration);
		}

	}
}

void UABCharacterSkillComponent::DrawDebugSkillCollision(const FVector& Center, const FQuat& Rotation, const FColor& Color, float LifeTime, float DepthPriority, float Thickness ) const
{
	UWorld* World = GetWorld();

	const FCollisionShape CollisionShape = GetCurrentSkillShape();

	switch (CollisionShape.ShapeType)
	{
		case ECollisionShape::Box:
			DrawDebugBox(World, Center, CollisionShape.GetBox(), Rotation, Color, false, LifeTime, DepthPriority, Thickness);
			break;
		case ECollisionShape::Capsule:
			DrawDebugCapsule(World, Center, CollisionShape.GetCapsuleHalfHeight(), CollisionShape.GetCapsuleRadius(), Rotation, Color, false, LifeTime, DepthPriority, Thickness);
			break;
		case ECollisionShape::Sphere:
			DrawDebugSphere(World, Center, CollisionShape.GetSphereRadius(), 12, Color, false, LifeTime, DepthPriority, Thickness);
			break;
		default:
			break;
	}
}

const float UABCharacterSkillComponent::GetCurrentSkillSpeedRate() const
{
	return SkillData->ComboActionData->AnimationSpeedRate * LastSkillParams.SkillSpeedRate;
}

const FCollisionShape UABCharacterSkillComponent::GetSkillShape(int32 SkillIdx) const
{
	return SkillData->GetCollisionShape(SkillIdx, FVector3f(LastSkillParams.SkillRangeForwardModifier, 0, 0), LastSkillParams.SkillExtentRate);
}

const float UABCharacterSkillComponent::GetSkillHitRange(int32 SkillIdx) const
{
	return OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius()
		+ GetSkillShape(SkillIdx).GetExtent().X;
}
