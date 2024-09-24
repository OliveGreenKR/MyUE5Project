// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/ABAnimationAttackInterface.h"
#include "ABCharacterBase.generated.h"

UENUM()
enum class ECharacterControlType : uint8
{
	None,
	Shoulder,
	Quater,
};

UCLASS()
class ARENABATTLE_API AABCharacterBase : public ACharacter , public IABAnimationAttackInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AABCharacterBase();

protected:
	virtual void SetCharacterControlData(const class UABCharacterControlData* InCharacterControlData);
	virtual void Tick(float DeltaTime) override;
protected:
	UPROPERTY(EditAnywhere, Category = CharacterControl, Meta = (AllowPrivateAccess = "true"))
	TMap<ECharacterControlType, class UABCharacterControlData*> CharacterControlManager;

#pragma region Skill
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill)
	TArray<TObjectPtr<class UABCharacterSkillComponent>> CharacterSkills;


	void ProcessComboCommand();
	void ComboActionBegin();
	void ComboActionEnd(class UAnimMontage* TargetMontage, bool IsProperlyEnded);
	void SetComboCheckTimer();
	/// <summary>
	/// Called When ComboTimer time out.
	/// </summary>
	virtual void ComboCheck();

	inline bool IsCombo() { return (CurrentCombo > 0); }
	UFUNCTION(BlueprintCallable, Category = "Combo")
	bool TrySetComboDirection(FVector InDesiredDirection);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category =  Animation)
	TObjectPtr<class UAnimMontage> ComboActionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UABComboActionData> ComboActionData;

	int32 CurrentCombo = 0;
	FTimerHandle ComboTimerHandle;
	bool bHasNextComboCommand = false;

private:
	//When ComboMontage Begins, try to rotate to this.
	FVector ComboDirection;
	bool bCanRedirection = false;
	bool bIsRedirectioning = false;

#pragma endregion
#pragma region Attack Hit
protected:
	virtual void PerformSkillHitCheck() override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> DeadMontage;

	virtual void SetDead();
	void PlayDeadAnimation();
#pragma endregion

};
