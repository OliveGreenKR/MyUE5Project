// Fill out your copyright notice in the Description page of Project Settings.
#include "Character/ABCharacterPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/ABCharacterControlData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Physics/ABCollision.h"
#include "Engine/DamageEvents.h"


AABCharacterPlayer::AABCharacterPlayer()
{
	// Camera
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Input
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionJumpRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_Jump.IA_Jump'"));
	if (nullptr != InputActionJumpRef.Object)
	{
		JumpAction = InputActionJumpRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputChangeActionControlRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_ChangeControl.IA_ChangeControl'"));
	if (nullptr != InputChangeActionControlRef.Object)
	{
		ChangeControlAction = InputChangeActionControlRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionShoulderMoveRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_ShoulderMove.IA_ShoulderMove'"));
	if (nullptr != InputActionShoulderMoveRef.Object)
	{
		ShoulderMoveAction = InputActionShoulderMoveRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionShoulderLookRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_ShoulderLook.IA_ShoulderLook'"));
	if (nullptr != InputActionShoulderLookRef.Object)
	{
		ShoulderLookAction = InputActionShoulderLookRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionQuaterMoveRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_QuaterMove.IA_QuaterMove'"));
	if (nullptr != InputActionQuaterMoveRef.Object)
	{
		QuaterMoveAction = InputActionQuaterMoveRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputAttackRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_Attack.IA_Attack'"));
	if (nullptr != InputAttackRef.Object)
	{
		AttackAction = InputAttackRef.Object;
	}

	CurrentCharacterControlType = ECharacterControlType::Quater;

}

void AABCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->JumpZVelocity = 500.f;  //default =700
	SetCharacterControl(CurrentCharacterControlType);
}

void AABCharacterPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	EnhancedInputComponent->BindAction(ChangeControlAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::ChangeCharacterControl);
	EnhancedInputComponent->BindAction(ShoulderMoveAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::ShoulderMove);
	EnhancedInputComponent->BindAction(ShoulderLookAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::ShoulderLook);
	EnhancedInputComponent->BindAction(QuaterMoveAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::QuaterMove);
	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::Attack);
}

void AABCharacterPlayer::ChangeCharacterControl()
{
	switch (CurrentCharacterControlType)
	{
		case ECharacterControlType::Quater:
		{
			SetCharacterControl(ECharacterControlType::Shoulder);
			break;
		}
		case ECharacterControlType::Shoulder:
		{
			SetCharacterControl(ECharacterControlType::Quater);
			break;
		}
		default:
		{
			SetCharacterControl(ECharacterControlType::Quater);
			break;
		}
	}
}

void AABCharacterPlayer::SetCharacterControl(ECharacterControlType NewCharacterControlType)
{
	UABCharacterControlData* NewCharacterControl = CharacterControlManager[NewCharacterControlType];
	check(NewCharacterControl);

	SetCharacterControlData(NewCharacterControl);

	//IMC 변경
	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();
		UInputMappingContext* NewMappingContext = NewCharacterControl->InputMappingContext;
		if (NewMappingContext)
		{
			Subsystem->AddMappingContext(NewMappingContext, 0);
		}
	}

	CurrentCharacterControlType = NewCharacterControlType;

}

void AABCharacterPlayer::SetCharacterControlData(const UABCharacterControlData* InCharacterControlData)
{
	Super::SetCharacterControlData(InCharacterControlData);

	CameraBoom->TargetArmLength				= InCharacterControlData->TargetArmLength;
	CameraBoom->SetRelativeRotation(InCharacterControlData->RelativeRotation);
	CameraBoom->bUsePawnControlRotation		= InCharacterControlData->bUsePawnControlRotation;
	CameraBoom->bInheritPitch				= InCharacterControlData->bInheritPitch;
	CameraBoom->bInheritYaw					= InCharacterControlData->bInheritYaw;
	CameraBoom->bInheritRoll				= InCharacterControlData->bInheritRoll;
	CameraBoom->bDoCollisionTest			= InCharacterControlData->bDoCollisionTest;
}

void AABCharacterPlayer::ShoulderMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.X);
	AddMovementInput(RightDirection, MovementVector.Y);
}

void AABCharacterPlayer::ShoulderLook(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void AABCharacterPlayer::QuaterMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	float InputSizeSquared = MovementVector.SquaredLength();
	float MovementVectorSize = 1.0f;
	float MovementVectorSizeSquared = MovementVector.SquaredLength();

	if (MovementVectorSizeSquared > 1.0f)
	{
		MovementVector.Normalize();
		MovementVectorSizeSquared = 1.0f;
	}
	else
	{
		MovementVectorSize = FMath::Sqrt(MovementVectorSizeSquared);
	}

	FVector MoveDirection = FVector(MovementVector.X, MovementVector.Y, 0.0f);
	GetController()->SetControlRotation(FRotationMatrix::MakeFromX(MoveDirection).Rotator());
	AddMovementInput(MoveDirection, MovementVectorSize);
}

void AABCharacterPlayer::Attack()
{
	ProcessComboCommand();
}

void AABCharacterPlayer::ComboCheck()
{
	Super::ComboCheck();

	FVector DesiredDirection;

	DesiredDirection = GetCharacterMovement()->GetLastInputVector();
	TrySetComboDirection(DesiredDirection);
}

void AABCharacterPlayer::PerformSkillHitCheck()
{
	//Collision check with 'ABAction' Trace Channel
	FHitResult OutHitResult;
	TArray<FHitResult> OutHitResults;
	//when analyzing tags, used Identifier info.
		//UnrealSCENE_QUERY_STAY로 tag 추가 가능
		//언리얼이 제공하는 분석툴에서 tag를 기준으로 analyze.
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);

	//TODO : Attack Info to Data Asset.
		//Ragne, Shape, Effective time, TraceMethod, Channel... etc
	const float AttackForward = 80.0f;
	const float AttackWidth = 80.0f;
	const float AttackHeight = 50.0f;
	
	const FVector HalfBox = FVector(AttackForward, AttackWidth, AttackHeight );
	const FQuat BoxRotation = FRotationMatrix::MakeFromXZ(GetActorForwardVector(), FVector::UpVector).ToQuat(); //forward to unit:x
	const FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + GetActorForwardVector() * AttackForward;

	bool HitDetected = GetWorld()->SweepMultiByChannel(OutHitResults, Start, End, BoxRotation, CCHANNEL_ABACTION, FCollisionShape::MakeBox(HalfBox), Params);

	if (bDrawDebug)
	{
		FVector AttackCenterPoint = Start + (End - Start) * 0.5f;
		FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;

		DrawDebugBox(GetWorld(), AttackCenterPoint, HalfBox, BoxRotation, DrawColor, false, 1.5f);

		// Draw debug boxes for each hit result
		for (const FHitResult& HitResult : OutHitResults)
		{
			DrawDebugBox(GetWorld(), HitResult.ImpactPoint, FVector(10.0f), FQuat::Identity, FColor::Blue, false, 1.5f);
		}
	}

}

