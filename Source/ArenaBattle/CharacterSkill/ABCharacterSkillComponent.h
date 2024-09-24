// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interface/ABAnimationAttackInterface.h"
#include "ABCharacterSkillComponent.generated.h"

DECLARE_DELEGATE(FCharacterSkillComponentOnSkillEndSignature);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARENABATTLE_API UABCharacterSkillComponent : public UActorComponent, public IABAnimationAttackInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UABCharacterSkillComponent();

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void BeginPlay() override;

	void ProcessSkill();
	inline void SetDrawDebug(bool InBool) { bSkillEnable = InBool; }
	inline void SetSkilEnable(bool InBool) { bIsRedirectioning = InBool; }

public:
	FCharacterSkillComponentOnSkillEndSignature OnSkillEnd;

protected:

	void SkillBegin();
	void SkillEnd(class UAnimMontage* TargetMontage, bool IsProperlyEnded);

	void SetComboCheckTimer();
	virtual void ComboCheck();
	inline bool IsCombo() const { return (CurrentCombo > 0); }

	UFUNCTION(BlueprintCallable, Category = "Combo")
	bool TrySetSkillDirection(FVector& InDesiredDirection);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UABSkillData> SkillData;

	int32 CurrentCombo = 0;
	FTimerHandle ComboTimerHandle;
	bool bHasNextComboCommand = false;

	virtual void PerformSkillHitCheck();
	void DrawDebugSkillCollision(const FVector& Center, const FQuat& Rotation, const FColor& Color, float LifeTime, float DepthPriority = (uint8)0U, float Thickness = 0.0f) const;

private:
	//When ComboMontage Begins, try to rotate to this.
	FVector ComboDirection;
	bool bCanRedirection = false;
	bool bIsRedirectioning = false;
	bool bSkillEnable = false;

private:
	bool bDrawDebug = false;
};
