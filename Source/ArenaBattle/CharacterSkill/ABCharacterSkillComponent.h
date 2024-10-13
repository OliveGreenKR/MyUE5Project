// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interface/ABAnimationAttackInterface.h"
#include "ABCharacterSkillComponent.generated.h"

DECLARE_DELEGATE(FOnCharacterSkillEndDelegate);
DECLARE_DELEGATE(FOnCharacterSkillBeginDelegate);

UCLASS( Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARENABATTLE_API UABCharacterSkillComponent : public UActorComponent, 
	public IABAnimationAttackInterface
{
	GENERATED_BODY()

public:	

	struct SkillParameters
	{
		float SkillSpeedRate = 1.0f;
		float SkillRangeForwardModifier = 0.0f;
		FVector3f SkillExtentRate = FVector3f::OneVector;
		float SkillDamageModifier = 0.0f;
		float SkillDamageMultiplier = 1.0f;
	};

	UABCharacterSkillComponent();

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnRegister() override;

	void ProcessSkill(const SkillParameters& InSkillParams, bool DrawDebug = false);

public:

	FOnCharacterSkillEndDelegate OnSkillEnd;
	FOnCharacterSkillBeginDelegate OnSkillBegin;

	FORCEINLINE bool IsCombo() const		{ return (CurrentCombo > 0); }
	FORCEINLINE int32 GetCurrentCombo()		{ return CurrentCombo; }
	FORCEINLINE void ResetCombo()			{ CurrentCombo = 0; }



protected:
	void SetComboNext();

private:
	void SkillBegin();
	void SkillEnd(class UAnimMontage* TargetMontage, bool IsProperlyEnded);
	void SetComboCheckTimer();
	virtual void CheckSkillCombo();

	UFUNCTION(BlueprintCallable, Category = "Combo")
	bool SetSkillDirection( const FVector InDesiredDirection);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UABSkillData> SkillData;

private:
	int32 CurrentCombo = 0;
	FTimerHandle ComboTimerHandle;
private:
	bool bHasNextComboCommand = false;

private:
	void DrawDebugSkillCollision(const FVector& Center, const FQuat& Rotation, const FColor& Color, float LifeTime, float DepthPriority = (uint8)0U, float Thickness = 0.0f) const;
	
#pragma region SkillExtent
public:
	//IABAnimationAttackInterface 
	virtual void PerformSkillHitCheck();
	FORCEINLINE const float GetNextSkillAttackRange() const { return GetSkillRange(CurrentCombo); }

protected:
	FORCEINLINE const FCollisionShape GetCurrentSkillShape() const { return GetSkillShape(CurrentCombo - 1); }
	const float GetCurrentSkillSpeedRate() const;
	FORCEINLINE const float GetCurrentSkillRange() const { return GetSkillRange(CurrentCombo - 1); }

private:
	const FCollisionShape GetSkillShape(int32 SkillIdx) const;
	const float GetSkillRange(int32 SkillIdx) const;
#pragma endregion

private:
	//Normalized. When ComboMontage Begins, try to rotate to this.
	FVector ComboDirection;
	bool bIsRedirectioning : 1 = false;
	bool bDrawDebug : 1 = false;

	SkillParameters LastSkillParams = SkillParameters();
	TObjectPtr<class ACharacter> OwnerCharacter;

};
