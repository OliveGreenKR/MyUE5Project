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
	PrimaryComponentTick.bCanEverTick = true;

	//static ConstructorHelpers::FObjectFinder<UAnimMontage> SkillMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/ArenaBattle/Animation/AM_ComboAttack.AM_ComboAttack'"));
	//if (SkillMontageRef.Object)
	//{
	//	SkillMontage = SkillMontageRef.Object;
	//}
	//static ConstructorHelpers::FObjectFinder<UABComboActionData> ComboActionDataRef(TEXT("/Script/ArenaBattle.ABComboActionData'/Game/ArenaBattle/CharacterAction/ABA_ComboAttack.ABA_ComboAttack'"));
	//if (ComboActionDataRef.Object)
	//{
	//	ComboActionData = ComboActionDataRef.Object;
	//}
	//static ConstructorHelpers::FObjectFinder<UABSkillData> SkillDataRef(TEXT("/Script/ArenaBattle.ABComboActionData'/Game/ArenaBattle/CharacterAction/ABA_ComboAttack.ABA_ComboAttack'"));
	//if (SkillDataRef.Object)
	//{
	//	SkillData = SkillDataRef.Object;
	//}

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
		}
	}

}

void UABCharacterSkillComponent::BeginPlay()
{
	Super::BeginPlay();

	check(Cast<ACharacter>(GetOwner()));
}

void UABCharacterSkillComponent::ProcessSkill()
{
	//First
	if (!IsCombo() && bSkillEnable)
	{
		SkillBegin();
		return;
	}

	bHasNextComboCommand = ComboTimerHandle.IsValid();
}

void UABCharacterSkillComponent::SkillBegin()
{
	float AttackSpeedRate = SkillData->ComboActionData->AnimationSpeedRate;
	USkeletalMeshComponent* SkeletalMesh = GetOwner()->GetComponentByClass<USkeletalMeshComponent>();
	
	UAnimInstance* AnimInstance = SkeletalMesh->GetAnimInstance();
	ensureMsgf(AnimInstance, TEXT("%s doesn't have AnimInstance"), *(GetOwner()->GetName()));

	CurrentCombo = 1;

	//Set deffault ComboDireciton 
	ComboDirection = GetOwner()->GetActorForwardVector();

	//TimerSet
	ComboTimerHandle.Invalidate();
	SetComboCheckTimer();

	//Play Anim
	AnimInstance->Montage_Play(SkillData->SkillMontage, AttackSpeedRate);

	//Set Montage End Deligate
	FOnMontageEnded EndDeligate;
	EndDeligate.BindUObject(this, &UABCharacterSkillComponent::SkillEnd);
	AnimInstance->Montage_SetEndDelegate(EndDeligate, SkillData->SkillMontage);
	
}

void UABCharacterSkillComponent::SkillEnd(UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
	ensure(CurrentCombo != 0);
	CurrentCombo = 0;
	bSkillEnable = false;
	OnSkillEnd.ExecuteIfBound();
}

void UABCharacterSkillComponent::SetComboCheckTimer()
{
	int32 ComboIndex = CurrentCombo - 1;
	UABComboActionData* ComboActionData = SkillData->ComboActionData;
	ensure(ComboActionData->EffectiveFrameCount.IsValidIndex(ComboIndex));

	float AttackSpeedRate = ComboActionData->AnimationSpeedRate;
	float ComboEffectiveTime = (ComboActionData->EffectiveFrameCount[ComboIndex] / ComboActionData->FrameRate) * SkillData->SkillMontage->GetSectionLength(ComboIndex);
	ComboEffectiveTime = ComboEffectiveTime / AttackSpeedRate;
	if (ComboEffectiveTime < 0)
	{
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &UABCharacterSkillComponent::ComboCheck, ComboEffectiveTime, false);
}

void UABCharacterSkillComponent::ComboCheck()
{
	ComboTimerHandle.Invalidate();
	//Check Next Command
	if (bHasNextComboCommand)
	{
		UABComboActionData* ComboActionData = SkillData->ComboActionData;
		USkeletalMeshComponent* SkeletalMesh = GetOwner()->GetComponentByClass<USkeletalMeshComponent>();
		UAnimInstance* AnimInstance = SkeletalMesh->GetAnimInstance();
		CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, ComboActionData->MaxComboCount);

		//Play Next Combo Animation
		FName NextSection = *FString::Printf(TEXT("%s%d"), *ComboActionData->MontageSectionNamePrefix, CurrentCombo);
		AnimInstance->Montage_JumpToSection(NextSection, SkillData->SkillMontage);
		SetComboCheckTimer();
		bCanRedirection = true;
		bHasNextComboCommand = false;
	}
}

bool UABCharacterSkillComponent::TrySetSkillDirection(FVector& InDesiredDirection)
{
	//during redirectioning, can not set direction.
	if (bCanRedirection == false)
	{
		return false;
	}
	//start redirectioning
	ComboDirection = InDesiredDirection;
	bIsRedirectioning = true;
	bCanRedirection = false;
	return true;
}

void UABCharacterSkillComponent::PerformSkillHitCheck()
{
	FHitResult OutHitResult;
	//tag :Attack
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, GetOwner());

	UCapsuleComponent* CapsuleComponent = GetOwner()->GetComponentByClass<UCapsuleComponent>();

	const FQuat BoxRotation = FRotationMatrix::MakeFromXZ(GetOwner()->GetActorForwardVector(), FVector::UpVector).ToQuat(); //forward to unit:x
	const FVector Start = GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * CapsuleComponent->GetScaledCapsuleRadius();
	const FVector End = Start + GetOwner()->GetActorForwardVector() * SkillData->SkillExtent.X;

	bool HitDetected = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, CCHANNEL_ABACTION, SkillData->GetCollisionShape(), Params);
	if (HitDetected)
	{

		FDamageEvent DamageEvent;
		ACharacter* Owner = Cast<ACharacter>(GetOwner());
		OutHitResult.GetActor()->TakeDamage(SkillData->SkillRawDamage, DamageEvent, Owner->GetController(), Owner);

	}

	if (bDrawDebug)
	{
		const FVector Origin = Start + (End - Start) * 0.5f;
		const FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;
		const FQuat DebugQuat = FRotationMatrix::MakeFromZ(GetOwner()->GetActorForwardVector()).ToQuat();

		DrawDebugSkillCollision(Origin, DebugQuat, DrawColor, 1.0f);
	}
}

void UABCharacterSkillComponent::DrawDebugSkillCollision(const FVector& Center, const FQuat& Rotation, const FColor& Color, float LifeTime, float DepthPriority, float Thickness ) const
{
	UWorld* World = GetWorld();

	const FCollisionShape CollisionShape = SkillData->GetCollisionShape();

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


