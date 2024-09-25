// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "Character/ABCharacterControlData.h"
#include "CharacterSkill/ABCharacterSkillComponent.h"
#include "Animation/AnimMontage.h"
#include "ABComboActionData.h"
#include "Physics/ABCollision.h"
#include "Engine/DamageEvents.h"

// Sets default values
AABCharacterBase::AABCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// Pawn
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_ABCAPSULE);

	// Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Mesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard'"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Game/ArenaBattle/Animation/Warriors/ABP_ABCharacter.ABP_ABCharacter_C"));
	if (AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}
	static ConstructorHelpers::FObjectFinder<UABCharacterControlData> QuarterRef(TEXT("/Script/ArenaBattle.ABCharacterControlData'/Game/ArenaBattle/CharacterControl/ABC_Quater.ABC_Quater'"));
	if (QuarterRef.Object)
	{
		CharacterControlManager.Add(ECharacterControlType::Quater, QuarterRef.Object);
	}
	static ConstructorHelpers::FObjectFinder<UABCharacterControlData> ShoulderRef(TEXT("/Script/ArenaBattle.ABCharacterControlData'/Game/ArenaBattle/CharacterControl/ABC_Shoulder.ABC_Shoulder'"));
	if (ShoulderRef.Object)
	{
		CharacterControlManager.Add(ECharacterControlType::Shoulder, ShoulderRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> DeadMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/ArenaBattle/Animation/Warriors/AM_Dead.AM_Dead'"));
	if (DeadMontageRef.Object)
	{
		DeadMontage = DeadMontageRef.Object;
	}


	//SKill Component
	BasicSkillComponent = CreateDefaultSubobject<UABCharacterSkillComponent>(TEXT("BasicSkillComponent"));
}

void AABCharacterBase::SetCharacterControlData(const UABCharacterControlData* InCharacterControlData)
{
	//Pawn
	bUseControllerRotationYaw = InCharacterControlData->bUseControllerRotationYaw;

	//CharacterMovement
	GetCharacterMovement()->bOrientRotationToMovement		= InCharacterControlData->bOrientRotationToMovement;
	GetCharacterMovement()->bUseControllerDesiredRotation	= InCharacterControlData->bUseControllerDesiredRotation;
	GetCharacterMovement()->RotationRate					= InCharacterControlData->RotationRate;

}

void AABCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
#pragma region Debug
	if (bDrawDebug)
	{
		//Draw debug Capsule collision
		FVector CapsuleLocation = GetCapsuleComponent()->GetComponentLocation();
		float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		float CapsuleRadius = GetCapsuleComponent()->GetScaledCapsuleRadius();
		FQuat CapsuleRotation = GetCapsuleComponent()->GetComponentQuat();

		DrawDebugCapsule(
			GetWorld(),
			CapsuleLocation,
			CapsuleHalfHeight,
			CapsuleRadius,
			CapsuleRotation,
			FColor::Green,
			false,
			-1.0f,  // Duration, -1 means it will be drawn for one frame
			0,
			1.0f    // Thickness of the lines
		);

		if (BasicSkillComponent)
		{
			BasicSkillComponent->SetDrawDebug(true);
		}

	}
#pragma endregion
	
}

void AABCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

float AABCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float InTrueDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	SetDead();

	return InTrueDamage;
}

void AABCharacterBase::DrawDebugForwardArrow(float InSeconds, FColor Color)
{
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation + GetActorForwardVector() * 50;

	DrawDebugDirectionalArrow(GetWorld(), StartLocation, EndLocation, 50.0f, Color, false, InSeconds, 0, 5.0f);

}

void AABCharacterBase::DrawDebugdArrow(FVector InDirection, float InSeconds, FColor Color)
{
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation + InDirection.GetSafeNormal() * 100.f;

	DrawDebugDirectionalArrow(GetWorld(), StartLocation, EndLocation, 50.0f, Color , false, InSeconds, 0, 5.0f);
}

void AABCharacterBase::SetDead()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	PlayDeadAnimation();
	SetActorEnableCollision(false);
}

void AABCharacterBase::PlayDeadAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	//AnimInstance->StopAllMontages(0.02f);
	AnimInstance->Montage_Play(DeadMontage, 1.0f);
}




