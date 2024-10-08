// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSkill/ABCharacterSkillComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "Animation/AnimMontage.h"
#include "Character/ABComboActionData.h"
#include "SkillData/ABSkillData.h"
#include "Physics/ABCollision.h"
#include "Engine/DamageEvents.h"

// Sets default values for this component's properties
UABCharacterSkillComponent::UABCharacterSkillComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UABSkillData> SkillDataRef(TEXT("/Script/ArenaBattle.ABSkillData'/Game/ArenaBattle/Skill/ABS_BasicSwordSkill.ABS_BasicSwordSkill'"));
	if (SkillDataRef.Object)
	{
		SkillData = SkillDataRef.Object;
	}
}

void UABCharacterSkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsRedirectioning)
	{
		FRotator CurrentRotation = GetOwner()->GetActorRotation();
		FRotator DesiredRotation = ComboDirection.Rotation();
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, DesiredRotation, DeltaTime, 8.0f);
		GetOwner()->SetActorRotation(NewRotation);

		//end rotate
		if (FMath::Abs((DesiredRotation - CurrentRotation).Yaw) < 0.1f)
		{
			bIsRedirectioning = false;
			GetOwner()->SetActorRotation(DesiredRotation);
		}
	}

}

void UABCharacterSkillComponent::OnRegister()
{
	Super::OnRegister();
	OwnerCharacter = CastChecked<ACharacter>(GetOwner());
}

void UABCharacterSkillComponent::ProcessSkill(const SkillParameters& InSkillParams, bool DrawDebug)
{
	//First
	if (!IsCombo() && !OwnerCharacter->GetCharacterMovement()->IsFalling())
	{
		LastSkillParams = InSkillParams;
		bDrawDebug = DrawDebug;
		SkillBegin();
		return;
	}

	bHasNextComboCommand = ComboTimerHandle.IsValid();
}

void UABCharacterSkillComponent::NextCombo()
{
	CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, SkillData->ComboActionData->MaxComboCount); 
}

void UABCharacterSkillComponent::SkillBegin()
{
	float AttackSpeedRate =
		SkillData->ComboActionData->AnimationSpeedRate *
		LastSkillParams.SkillSpeedRate;

	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	ensureMsgf(AnimInstance, TEXT("%s doesn't have AnimInstance"), *(GetOwner()->GetName()));

	CurrentCombo = 1;
	
	UCharacterMovementComponent* Movement = OwnerCharacter->GetCharacterMovement();
	
	//character stop
	Movement->SetMovementMode(EMovementMode::MOVE_None);

	//Set deffault ComboDireciton 
	SetSkillDirection(Movement->GetLastInputVector());

	//TimerSet
	ComboTimerHandle.Invalidate();
	SetComboCheckTimer();

	//Play Anim
	AnimInstance->Montage_Play(SkillData->SkillMontage, AttackSpeedRate);

	//Set Montage End Deligate
	FOnMontageEnded EndDeligate;
	EndDeligate.BindUObject(this, &UABCharacterSkillComponent::SkillEnd);
	AnimInstance->Montage_SetEndDelegate(EndDeligate, SkillData->SkillMontage);
	OnSkillBegin.ExecuteIfBound();
}

void UABCharacterSkillComponent::SkillEnd(UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
	//character move
	UCharacterMovementComponent* Movement = OwnerCharacter->GetCharacterMovement();
	Movement->SetMovementMode(EMovementMode::MOVE_Walking);

	ensure(CurrentCombo != 0);
	ResetCombo();
	bDrawDebug = false;
	OnSkillEnd.ExecuteIfBound();
}

void UABCharacterSkillComponent::SetComboCheckTimer()
{
	int32 ComboIndex = GetCurrentCombo() - 1;
	UABComboActionData* ComboActionData = SkillData->ComboActionData;
	ensure(ComboActionData->EffectiveFrameCount.IsValidIndex(ComboIndex));

	float AttackSpeedRate = ComboActionData->AnimationSpeedRate * LastSkillParams.SkillSpeedRate;
	float ComboEffectiveTime = (ComboActionData->EffectiveFrameCount[ComboIndex] / ComboActionData->FrameRate) * SkillData->SkillMontage->GetSectionLength(ComboIndex);
	ComboEffectiveTime = ComboEffectiveTime / AttackSpeedRate;
	if (ComboEffectiveTime < 0)
	{
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &UABCharacterSkillComponent::CheckSkillCombo, ComboEffectiveTime, false);
}

void UABCharacterSkillComponent::CheckSkillCombo()
{
	ComboTimerHandle.Invalidate();
	//Check Next Command
	if (bHasNextComboCommand)
	{
		UABComboActionData* ComboActionData = SkillData->ComboActionData;
		UAnimInstance * AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
		UCharacterMovementComponent* Movement = OwnerCharacter->GetCharacterMovement();
		//NextCombo Index
		NextCombo();

		//Play Next Combo Animation
		FName NextSection = *FString::Printf(TEXT("%s%d"), *ComboActionData->MontageSectionNamePrefix, GetCurrentCombo());
		AnimInstance->Montage_JumpToSection(NextSection, SkillData->SkillMontage);
		SetComboCheckTimer();
		
		//Skill Redirection
		SetSkillDirection(Movement->GetLastInputVector());
		bHasNextComboCommand = false;
	}
}

bool UABCharacterSkillComponent::SetSkillDirection( const FVector InDesiredDirection)
{
	ComboDirection = (InDesiredDirection == FVector::ZeroVector) 
		? GetOwner()->GetActorForwardVector() 
		: InDesiredDirection;

	bIsRedirectioning = true;
	return true;
}

void UABCharacterSkillComponent::PerformSkillHitCheck()
{
	TArray<FHitResult> OutHitResults;
	//tag :Attack
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, GetOwner());

	UCapsuleComponent* CapsuleComponent = OwnerCharacter->GetCapsuleComponent();

	const FSkillDataPerMotion& NowMotionData = SkillData->GetSkillMotionData(GetCurrentCombo() - 1);
	const FQuat CollisionRotation = FRotationMatrix::MakeFromXZ(ComboDirection, FVector::UpVector).ToQuat(); //forward to unit:x
	const FVector Start = GetOwner()->GetActorLocation() + ComboDirection * CapsuleComponent->GetScaledCapsuleRadius();
	const FCollisionShape CollisionShape = GetCurrentSkillShape();
	const FVector End = Start + ComboDirection * CollisionShape.GetExtent().X;

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

const FCollisionShape UABCharacterSkillComponent::GetCurrentSkillShape() const
{
	return SkillData->GetCollisionShape(CurrentCombo - 1,FVector3f(LastSkillParams.SkillRangeForwardModifier, 0, 0), LastSkillParams.SkillExtentRate);
}


