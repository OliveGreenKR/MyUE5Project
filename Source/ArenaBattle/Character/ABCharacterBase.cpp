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
#include "UI/ABWidgetComponent.h"
#include "CharacterStat/ABCharacterStatComponent.h"
#include "UI/ABHpBarWidget.h"
#include "Item/ABItems.h"
#include "GameData/ABCharacterStat.h"

DEFINE_LOG_CATEGORY(LogABCharacter);

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
	GetCharacterMovement()->JumpZVelocity = 500.f; //default 700
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

	//SkillComponents
	BasicSkillComponent = CreateDefaultSubobject<UABCharacterSkillComponent>(TEXT("BasicSkill"));

	//Stats
	Stat = CreateDefaultSubobject<UABCharacterStatComponent>(TEXT("Stat"));

	//HpBar
	HpBar = CreateDefaultSubobject<UABWidgetComponent>(TEXT("Widget"));
	HpBar->SetupAttachment(GetMesh());
	HpBar->SetRelativeLocation(FVector(0, 0, 180.0f));
	static ConstructorHelpers::FClassFinder<UUserWidget> HpBarWidgetRef(TEXT("/Game/ArenaBattle/UI/WBP_HpBar.WBP_HpBar_C"));
	if (HpBarWidgetRef.Class)
	{
		HpBar->SetWidgetClass(HpBarWidgetRef.Class);
		HpBar->SetWidgetSpace(EWidgetSpace::Screen);
		HpBar->SetDrawSize(FVector2D(150.0f, 15.0f));
		HpBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	//Item Actions
	TakeItemActions.Reserve(3);
	TakeItemActions.Add(EItemType::Weapon, FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AABCharacterBase::EquipWeapon)));
	TakeItemActions.Add(EItemType::Potion, FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AABCharacterBase::DrinkPotion)));
	TakeItemActions.Add(EItemType::Scroll, FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AABCharacterBase::ReadScroll)));

	//Equipment
	WeaponSocket = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	WeaponSocket->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));
}

void AABCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//Set UI Location
	HpBar->SetRelativeLocation(FVector(0, 0, GetMesh()->Bounds.BoxExtent.Z * 2.0f + 20.0f));

	//Binding Stat Delegates
	Stat->OnHpZero.AddUObject(this, &AABCharacterBase::SetDead);
	Stat->OnStatChanged.AddUObject(this, &AABCharacterBase::OnStatChanged);
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

#pragma region Manual Redirection
	if (bManualReDireciton)
	{
		if (ManualDirection.IsNearlyZero(KINDA_SMALL_NUMBER))
		{
			bManualReDireciton = false;
		}

		FRotator TargetRot = FRotationMatrix::MakeFromX(ManualDirection).Rotator();
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRot, DeltaTime, ManualTurnInterpSpeed));

		if(GetActorRotation().Equals(TargetRot, KINDA_SMALL_NUMBER))
		{
			bManualReDireciton = false;
		}
	}
#pragma endregion 
	
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

	}
#pragma endregion
}

float AABCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float InTrueDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Stat->ApplyDamage(DamageAmount);

	if (bDrawDebug)
	{
		const FVector TopOfCapsule = GetActorLocation() + FVector(0.f, 0.f, GetSimpleCollisionHalfHeight() + 10.0f);
		const FColor DebugColor = FColor::Red;
		FString DebugText = FString::Printf(TEXT("Taken Damage : %.3f "), DamageAmount);
		DrawDebugString(GetWorld(), TopOfCapsule, DebugText, nullptr, DebugColor, 1.f, true, 3.0f);
	}
	//for cancel test.
	BasicSkillComponent->CancelSkill();

	return InTrueDamage;
}
const int32 AABCharacterBase::GetLevel()
{
	return Stat->GetCurrentLevel();
}

void AABCharacterBase::SetLevel(int32 InNewLevel)
{
	Stat->SetLevelStat(InNewLevel);
}

void AABCharacterBase::OnStatChanged(const FABCharacterStat& Base, const FABCharacterStat& Modifier)
{
	//subscriber for  Stat::FOnStatChangeDelegate
	FABCharacterStat TotalStat = Stat->GetTotalStat();
	float MovementSpeed = TotalStat.MovementSpeed;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
	return;
}

void AABCharacterBase::SetupCharacterWidget(UABUserWidget* InUserWidget)
{
	//TODO :  BidnMap< WidgetClass, BindFucntion >
	UABHpBarWidget* HpBarWidget = Cast<UABHpBarWidget>(InUserWidget);
	if (HpBarWidget)
	{
		HpBarWidget->SetMaxHp(Stat->GetTotalStat().MaxHp);
		HpBarWidget->UpdateHpBar(Stat->GetCurrentHp());
		Stat->OnHpChanged.AddUObject(HpBarWidget, &UABHpBarWidget::UpdateHpBar);
	}
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
	if (BasicSkillComponent)
	{
		BasicSkillComponent->CancelSkill();
		BasicSkillComponent->Deactivate();
	}
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	PlayDeadAnimation();
	SetActorEnableCollision(false);
	HpBar->SetHiddenInGame(true);
}

void AABCharacterBase::PlayDeadAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(DeadMontage, 1.0f);
}

void AABCharacterBase::TakeItem(UABItemData* InItemData)
{
	if (InItemData && TakeItemActions.Contains(InItemData->Type))
	{
		TakeItemActions[InItemData->Type].ItemDelegate.ExecuteIfBound(InItemData);
	}
}

void AABCharacterBase::DrinkPotion(UABItemData* InItemData)
{
	//draw debug string
	{
		FVector CapsuleLocation = GetCapsuleComponent()->GetComponentLocation();
		float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		FVector CapsuleTopLocation = CapsuleLocation + FVector(0.0f, 0.0f, CapsuleHalfHeight + 30.f);
		DrawDebugString(GetWorld(), CapsuleTopLocation, TEXT("Potion"), nullptr, FColor::Black, 1.0f, false, 1.5f);
	}

	UABPotionItemData* PotionItemData = Cast<UABPotionItemData>(InItemData);
	if (PotionItemData)
	{
		Stat->AddCurrentHp(PotionItemData->HealAmount);
	}

}
void AABCharacterBase::EquipWeapon(UABItemData* InItemData)
{
	UABWeaponItemData* WeaponItemData = Cast<UABWeaponItemData>(InItemData);
	if (WeaponItemData)
	{
		if (WeaponItemData->WeaponMesh.IsPending())
		{
			WeaponItemData->WeaponMesh.LoadSynchronous();
		}

		WeaponSocket->SetSkeletalMesh(WeaponItemData->WeaponMesh.Get());
		Stat->SetModifierStat(WeaponItemData->ModifierStat);
	}
}

void AABCharacterBase::ReadScroll(UABItemData* InItemData)
{
	//draw debug string
	{
		FVector CapsuleLocation = GetCapsuleComponent()->GetComponentLocation();
		float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		FVector CapsuleTopLocation = CapsuleLocation + FVector(0.0f, 0.0f, CapsuleHalfHeight + 30.f);
		DrawDebugString(GetWorld(), CapsuleTopLocation, TEXT("Scroll"), nullptr, FColor::Black, 1.0f, false, 1.5f);
	}

	UABScrollItemData* ScrollItemData = Cast<UABScrollItemData>(InItemData);
	if (ScrollItemData)
	{
		Stat->AddBaseStat(ScrollItemData->BaseStat);
	}

}





