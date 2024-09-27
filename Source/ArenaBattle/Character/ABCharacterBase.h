// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ABCharacterBase.generated.h"

UENUM()
enum class ECharacterControlType : uint8
{
	None,
	Shoulder,
	Quater,
};

UCLASS()
class ARENABATTLE_API AABCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AABCharacterBase();

protected:
	virtual void SetCharacterControlData(const class UABCharacterControlData* InCharacterControlData);
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	UPROPERTY(EditAnywhere, Category = CharacterControl, Meta = (AllowPrivateAccess = "true"))
	TMap<ECharacterControlType, class UABCharacterControlData*> CharacterControlManager;


#pragma region Skill Components
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UABCharacterSkillComponent> BasicSkill;
#pragma endregion

#pragma region Stat Component
protected:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = Stat, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class UABCharacterStatComponent> Stat;
#pragma endregion

#pragma region UI Widget Components
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Widget, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UABWidgetComponent> HpBar;
#pragma endregion

#pragma region Debug
protected:
	void DrawDebugForwardArrow(float InSeconds, FColor Color = FColor::Red);
	void DrawDebugdArrow(FVector InDirection, float InSeconds, FColor Color = FColor::Red);
protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = Debug )
	uint8 bDrawDebug :1 = 0 ;
#pragma endregion

#pragma region Dead
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> DeadMontage;

	virtual void SetDead();
	void PlayDeadAnimation();

protected:
	float DeadEventDelayTime = 5.0f;
#pragma endregion

};
