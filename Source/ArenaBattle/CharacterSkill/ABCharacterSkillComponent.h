// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interface/ABAnimationAttackInterface.h"
#include "Interface/ABSkillExecutorInterface.h"
#include "ABCharacterSkillComponent.generated.h"

UCLASS( Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARENABATTLE_API UABCharacterSkillComponent : public UActorComponent, 
	public IABAnimationAttackInterface, public IABSkillExecutorInterface
{
	GENERATED_BODY()

public:	

	UABCharacterSkillComponent();

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;

#pragma region SkillExecutions
public:
	//Inherited IABSkillExecutionInterface
	virtual void ExecuteSkill(const SkillParameters& InSkillParams, bool DrawDebug = false) override;
	virtual void CancelSkill(float InBlendInTime = 0.0f) override;
	virtual const float GetSkillRange() const;

	FORCEINLINE const bool IsCombo() const		{ return (CurrentCombo > 0); }
	FORCEINLINE const int32 GetCurrentCombo()	{ return CurrentCombo; }
	FORCEINLINE void ResetCombo()				{ CurrentCombo = 0; }
	FORCEINLINE const bool IsSkillCoolDown()    { return CoolDownTimerHandle.IsValid(); }

	UFUNCTION(BlueprintCallable)
	void SetCancelable(const bool InBool); 

	UFUNCTION(BlueprintCallable)
	float GetRemainSkillCoolDown();
protected:
	void ProcessSkill(const SkillParameters& InSkillParams, bool DrawDebug = false);
	void SetComboNext();

private:
	void SkillBegin();

	UFUNCTION()
	void OnSkillMontageEnd(class UAnimMontage* TargetMontage, bool IsProperlyEnded);
	void SkillEnd();
	void SetComboCheckTimer();
	void SetCoolDownTimer();
	void CheckSkillCombo();
	void OnCoolDownEnd();
	UFUNCTION()
	void OnComboIgnoreEnd(float ComboEffectiveTime);
	void OnComboTimerEnd();

private: 
	bool isCancelable : 1 = true;
	UPROPERTY(VisibleAnywhere)
	int32 CurrentCombo = 0;
	FTimerHandle ComboTimerHandle;          //enable cobo input 
	FTimerHandle ComboIgnoreTimerHandle;    //combo input igonred time
	FTimerHandle CoolDownTimerHandle;

	UPROPERTY(VisibleAnywhere)
	bool bHasNextComboCommand = false;
	SkillParameters LastSkillParams = SkillParameters();

	TObjectPtr<class ACharacter> OwnerCharacter;
#pragma endregion
#pragma region SkillExtent
public:
	//IABAnimationAttackInterface 
	virtual void PerformSkillHitCheck();
	

protected:
	FORCEINLINE const float GetNextSkillHitRange() const { return GetSkillHitRange(CurrentCombo); }
	FORCEINLINE const FCollisionShape GetCurrentSkillShape() const { return GetSkillShape(CurrentCombo - 1); }
	const float GetCurrentSkillSpeedRate() const;
	FORCEINLINE const float GetCurrentSkillHitRange() const { return GetSkillHitRange(CurrentCombo - 1); }
private:
	const FCollisionShape GetSkillShape(int32 SkillIdx) const;
	const float GetSkillHitRange(int32 SkillIdx) const;
#pragma endregion

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UABSkillData> SkillData;

#pragma region Debug
private:
	void DrawDebugSkillCollision(const FVector& Center, const FQuat& Rotation, const FColor& Color, float LifeTime, float DepthPriority = (uint8)0U, float Thickness = 0.0f) const;

private:
	bool bDrawDebug : 1 = false;
#pragma endregion
};
