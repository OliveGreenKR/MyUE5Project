// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/ABCharacterWidgetInterface.h"
#include "Interface/ABCharacterItemInterface.h"
#include "ABCharacterBase.generated.h"

enum class EItemType : uint8;

UENUM()
enum class ECharacterControlType : uint8
{
	None,
	Shoulder,
	Quater,
};

DECLARE_LOG_CATEGORY_EXTERN(LogABCharacter, Log, All);
DECLARE_DELEGATE_OneParam(FOnTakeItemDelegate, class UABItemData* /*InItemData*/);

USTRUCT(BlueprintType)
struct FTakeItemDelegateWrapper
{
	GENERATED_BODY()
	FTakeItemDelegateWrapper() {}
	FTakeItemDelegateWrapper(const FOnTakeItemDelegate& InItemDelegate) : ItemDelegate(InItemDelegate) {}
	FOnTakeItemDelegate ItemDelegate;
};

UCLASS()
class ARENABATTLE_API AABCharacterBase : public ACharacter, public IABCharacterWidgetInterface, public IABCharacterItemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AABCharacterBase();
	virtual void PostInitializeComponents() override;

protected:
	virtual void SetCharacterControlData(const class UABCharacterControlData* InCharacterControlData);
	
	virtual void Tick(float DeltaTime) override;
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
protected:
	UPROPERTY(EditAnywhere, Category = CharacterControl, Meta = (AllowPrivateAccess = "true"))
	TMap<ECharacterControlType, class UABCharacterControlData*> CharacterControlManager;
#pragma region Hit Reaction
protected:
	virtual void OnHit();

private:
	void PlayHitReaction(float InBlendInTime = 1.0f);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> HitReactionMontage;

#pragma endregion
#pragma region Manual Redireciton
protected :
	void SetManualDirection(const FVector& InDirection) 
	{
		ManualDirection = InDirection.GetSafeNormal2D(SMALL_NUMBER, GetActorForwardVector()); 
	}

	FORCEINLINE void StartManualDirection() { bManualReDireciton = true; }
	FORCEINLINE void EndManualDirection() { bManualReDireciton = false; ManualDirection = FVector::ZeroVector; }

	float ManualTurnInterpSpeed = 4.0f;
private:
	FVector ManualDirection = FVector::ZeroVector;
	bool bManualReDireciton : 1 = false;
#pragma endregion
#pragma region Skill Components
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UABCharacterSkillComponent> BasicSkillComponent;
#pragma endregion

#pragma region Stat Component
public :
	const int32 GetLevel();
	void SetLevel(int32 InNewLevel);
	void OnStatChanged(const struct FABCharacterStat& Base, const struct FABCharacterStat& Modifier);

protected:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = Stat, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class UABCharacterStatComponent> Stat;
#pragma endregion

#pragma region UI Widget Components
protected:
	//Inherited IABCharacterWidgetInterface
	void SetupCharacterWidget(UABUserWidget* InUserWidget) override;

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
	float DeadEventDelayTime = 2.0f;
#pragma endregion

#pragma region Item
protected:
	// Inherited via IABCharacterItemInterface
	virtual void TakeItem(UABItemData* InItemData) override;

	virtual void EquipWeapon(class UABItemData* InItemData);
	virtual void DrinkPotion(class UABItemData* InItemData);
	virtual void ReadScroll(class UABItemData* InItemData);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipment, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USkeletalMeshComponent> WeaponSocket;

	UPROPERTY()
	TMap<EItemType,FTakeItemDelegateWrapper> TakeItemActions;


#pragma endregion

};
