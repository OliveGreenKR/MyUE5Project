// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ABCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHpZeroDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHpChangedDelegate, float);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARENABATTLE_API UABCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UABCharacterStatComponent();

public:
	FORCEINLINE float GetMaxHp()		const	{ return MaxHp; }
	FORCEINLINE float GetCurrentHp()	const	{ return CurrentHp; }
	float ApplyDamage(float InDamage);

public:
	FOnHpZeroDelegate OnHpZero;
	FOnHpChangedDelegate OnHpChanged;

protected:
	//CurrentHp must be set by this function. forcely set Hp in range [0:MaxHp]
	void SetCurrentHp(float NewHp);
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditInstanceOnly, Category = Stat)
	float MaxHp;
private:
	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat)
	float CurrentHp;

};
