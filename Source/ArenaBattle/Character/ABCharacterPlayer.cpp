// Fill out your copyright notice in the Description page of Project Settings.
#include "Character/ABCharacterPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/ABCharacterControlData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CharacterSkill/ABCharacterSkillComponent.h"
#include "CharacterStat/ABCharacterStatComponent.h"
#include "UI/ABHUDWidget.h"
#include "Interface/ABGameInterface.h"
#include "GameFramework/GameMode.h"

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

void AABCharacterPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AABCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		EnableInput(PlayerController);

	}
	BasicSkillComponent->Activate();
	SetCharacterControl(CurrentCharacterControlType);
}

void AABCharacterPlayer::SetDead()
{
	Super::SetDead();

	//is Controlled Player
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		//disable input
		DisableInput(PlayerController);


		//request Dead Sequence to GameMode
		IABGameInterface* ABGameMode = Cast<IABGameInterface>(GetWorld()->GetAuthGameMode());
		if (ABGameMode)
		{
			ABGameMode->OnPlayerDead();
		}
	}

	
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

void AABCharacterPlayer::OnPreHit()
{
	Super::OnPreHit();

	APlayerController* PlayerController = Cast< APlayerController>(GetController());
	if (PlayerController)
	{
		DisableInput(PlayerController);
	}
	
	
}

void AABCharacterPlayer::OnPostHit(UAnimMontage* Montage, bool bInterrupted)
{
	Super::OnPostHit(Montage, bInterrupted);
	APlayerController* PlayerController = Cast< APlayerController>(GetController());
	if (PlayerController)
	{
		EnableInput(PlayerController);
	}
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
	//Load Data Asset From ControlDataMap (Edited in BP)
	UABCharacterControlData* NewCharacterControl = CharacterControlManager[NewCharacterControlType];
	check(NewCharacterControl);

	SetCharacterControlData(NewCharacterControl);

	//IMC ����
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
	if (!GetCharacterMovement()->IsFalling() && BasicSkillComponent)
	{
		using SkillParameters = IABSkillExecutorInterface::SkillParameters;

		SkillParameters OutSkillParams = SkillParameters();
		FABCharacterStat TotalStat = Stat->GetTotalStat();

		OutSkillParams.SkillDamageModifier = TotalStat.Attack;
		OutSkillParams.SkillExtentRate = FVector3f(TotalStat.AttackRangeRate);
		OutSkillParams.SkillRangeForwardModifier = TotalStat.AttackRangeForward;
		OutSkillParams.SkillSpeedRate = TotalStat.AttackSpeedRate;
		BasicSkillComponent->OnSkillBegin.BindLambda([&]() { this->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None); });
		BasicSkillComponent->OnSkillEnd.BindLambda([&]() { this->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking); }); 
		if (!BasicSkillComponent->OnSkillSeqBegin.IsBoundToObject(this))
		{
			BasicSkillComponent->OnSkillSeqBegin.AddUObject(this, &AABCharacterPlayer::SetSkillDiretion);
		}
		if (!BasicSkillComponent->OnSkillSeqEnd.IsBoundToObject(this))
		{
			BasicSkillComponent->OnSkillSeqEnd.AddUObject(this, &AABCharacterPlayer::EndManualDirection);
		}
		BasicSkillComponent->ExecuteSkill(OutSkillParams, bDrawDebug);
	}
}

void AABCharacterPlayer::SetSkillDiretion()
{
	ManualTurnInterpSpeed = 8.0f;
	StartManualDirection();
	SetManualDirection(GetCharacterMovement()->GetLastInputVector());
}

void AABCharacterPlayer::SetupHUDWidget(UABHUDWidget* InHUDWidget)
{
	if (InHUDWidget)
	{
		//binding data to HUD Widgets
		InHUDWidget->UpdateStat(Stat->GetBaseStat(), Stat->GetModifierStat());
		InHUDWidget->UpdateHpBar(Stat->GetCurrentHp());

		Stat->OnStatChanged.AddUObject(InHUDWidget, &UABHUDWidget::UpdateStat);
		Stat->OnHpChanged.AddUObject(InHUDWidget, &UABHUDWidget::UpdateHpBar);
	}
}



