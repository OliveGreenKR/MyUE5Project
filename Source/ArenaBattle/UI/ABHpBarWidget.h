// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ABUserWidget.h"
#include "ABHpBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABHpBarWidget : public UABUserWidget
{
	GENERATED_BODY()
	
public:
	UABHpBarWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	void UpdateCurrentHP(float NewCurrentHp);
	void UpdateMaxHp(const struct FABCharacterStat& BaseStat, const struct FABCharacterStat& ModifierStat);

protected:
	void UpdateHpBar();
	void SetCurrentHp(float InHp) { CurrentHp = InHp; UpdateHpBar(); }
	void SetMaxtHp(float InHp) { MaxHp = InHp; UpdateHpBar(); }
	FORCEINLINE float GetCurrentHp() { return CurrentHp; }
	FORCEINLINE float GetMaxHp() { return MaxHp; }
	


protected:
	UPROPERTY()
	TObjectPtr<class UProgressBar> HpProgressBar;

	UPROPERTY()
	TObjectPtr<class UTextBlock> HpStatText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName HpBarName = FName(TEXT("PbHpBar"));

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName HpTextName = FName(TEXT("TxtHpStat"));

private:
	UPROPERTY()
	float MaxHp = -1.0f;
	UPROPERTY()
	float CurrentHp = -1.0f;
};
