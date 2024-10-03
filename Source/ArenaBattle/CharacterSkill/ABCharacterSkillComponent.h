// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interface/ABAnimationAttackInterface.h"
#include "ABCharacterSkillComponent.generated.h"

DECLARE_DELEGATE(FOnCharacterSkillEndDelegate);

UCLASS( Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARENABATTLE_API UABCharacterSkillComponent : public UActorComponent, public IABAnimationAttackInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UABCharacterSkillComponent();

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnRegister() override;

	void ProcessSkill();
	inline void SetDrawDebug(bool InBool) { bDrawDebug = InBool; }

	//IABAnimationAttackInterface 
	virtual void PerformSkillHitCheck();

public:

	FOnCharacterSkillEndDelegate OnSkillEnd;

protected:

	void SkillBegin();
	void SkillEnd(class UAnimMontage* TargetMontage, bool IsProperlyEnded);

	void SetComboCheckTimer();
	virtual void ComboCheck();
	inline bool IsCombo() const { return (CurrentCombo > 0); }

	UFUNCTION(BlueprintCallable, Category = "Combo")
	bool SetSkillDirection( const FVector InDesiredDirection);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UABSkillData> SkillData;

	int32 CurrentCombo = 0;
	FTimerHandle ComboTimerHandle;
	bool bHasNextComboCommand = false;

	
private:
	void DrawDebugSkillCollision(const FVector& Center, const FQuat& Rotation, const FColor& Color, float LifeTime, float DepthPriority = (uint8)0U, float Thickness = 0.0f) const;

private:
	TObjectPtr<class AABCharacterBase> OwnerCharacter = nullptr;

	//When ComboMontage Begins, try to rotate to this.
	FVector ComboDirection;
	bool bIsRedirectioning = false;
	bool bDrawDebug = false;
};
