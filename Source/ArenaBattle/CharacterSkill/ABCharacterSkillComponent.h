// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interface/ABAnimationAttackInterface.h"
#include "Interface/ABSkillExecutorInterface.h"
#include "ABCharacterSkillComponent.generated.h"

DECLARE_DELEGATE(FOnCharacterSkillEndDelegate);
DECLARE_DELEGATE(FOnCharacterSkillBeginDelegate);

UCLASS( Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARENABATTLE_API UABCharacterSkillComponent : public UActorComponent, 
	public IABAnimationAttackInterface, public IABSkillExecutorInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UABCharacterSkillComponent();

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnRegister() override;

	//IABAnimationAttackInterface 
	virtual void PerformSkillHitCheck();

	inline void SetDrawDebug(bool InBool) { bDrawDebug = InBool; }
public:

	FOnCharacterSkillEndDelegate OnSkillEnd;
	FOnCharacterSkillBeginDelegate OnSkillBegin;

private:
	void SkillBegin();
	void SkillEnd(class UAnimMontage* TargetMontage, bool IsProperlyEnded);

	void SetComboCheckTimer();
	virtual void CheckSkillCombo();
	inline bool IsCombo() const { return (CurrentCombo > 0); }

	UFUNCTION(BlueprintCallable, Category = "Combo")
	bool SetSkillDirection( const FVector InDesiredDirection);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UABSkillData> SkillData;

	int32 CurrentCombo = 0;
	FTimerHandle ComboTimerHandle;
private:
	bool bHasNextComboCommand = false;

private:
	void DrawDebugSkillCollision(const FVector& Center, const FQuat& Rotation, const FColor& Color, float LifeTime, float DepthPriority = (uint8)0U, float Thickness = 0.0f) const;

public:
	// Inherited via IABSkillExecutorInterface
	void ExecuteSkill(const SkillParameters& InSkillParams) override;

private:
	void ProcessSkill();

	//When ComboMontage Begins, try to rotate to this.
	FVector ComboDirection;
	bool bIsRedirectioning = false;
	bool bDrawDebug = false;

	SkillParameters LastSkillParams = SkillParameters();
	TObjectPtr<class ACharacter> OwnerCharacter;


};
