// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameData/ABCharacterStat.h"
#include "ABCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHpZeroDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHpChangedDelegate, float);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStatChangeedDelegate,
									 const FABCharacterStat& /*BaseStat*/,
									 const FABCharacterStat& /*ModifierStat*/);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARENABATTLE_API UABCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UABCharacterStatComponent();

public:
	void SetLevelStat(int32 InNewLevel);
	void SetModifierStat(const FABCharacterStat& InModifierStat)	{ ModifierStat = InModifierStat; OnStatChanged.Broadcast(BaseStat,ModifierStat); }
	void SetBaseStat(const FABCharacterStat& InBaseStat)			{ BaseStat = InBaseStat; OnStatChanged.Broadcast(BaseStat,ModifierStat); }
	
	FORCEINLINE float GetCurrentLevel() const { return CurrentLevel; }
	FORCEINLINE const FABCharacterStat& GetBaseStat() const						{ return BaseStat; }
	FORCEINLINE const FABCharacterStat& GetModifierStat() const					{ return ModifierStat; }
	FORCEINLINE FABCharacterStat GetTotalStat() const							{ return BaseStat + ModifierStat; }
	FORCEINLINE float GetCurrentHp() const										{ return CurrentHp; }
	
	FORCEINLINE void AddCurrentHp(float InHpDelta)								{ SetCurrentHp(CurrentHp + InHpDelta); }
	void AddBaseStat(const FABCharacterStat& InDeltaStat);
	float ApplyDamage(float InDamage);

public:
	FOnHpZeroDelegate OnHpZero;
	FOnHpChangedDelegate OnHpChanged;
	FOnStatChangeedDelegate OnStatChanged;

protected:
	virtual void InitializeComponent() override;

private:
	//CurrentHp must be set by this function. forcely set Hp in range [0:MaxHp] and BroadCast 'OnHpChanged'
	void SetCurrentHp(float NewHp);

protected:
	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat)
	float CurrentHp;

	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat)
	float CurrentLevel;

private:
	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	FABCharacterStat BaseStat;

	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	FABCharacterStat ModifierStat;

};
