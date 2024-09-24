// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ABSkillData.generated.h"

UENUM(BlueprintType)
enum class ESkillCollisionShape : uint8
{
	Box UMETA(DisplayName = "Box"),
	Sphere UMETA(DisplayName = "Sphere"),
	Capsule UMETA(DisplayName = "Capsule"),
};

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABSkillData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:

	UABSkillData();

	const struct FCollisionShape GetCollisionShape() const;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Name)
	FString SkillName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill)
	float SkillCoolTime = -1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill, meta = (ToolTip = "Forward, Width, Height"))
	FVector3f SkillExtent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill)
	float SkillRawDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> SkillMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UABComboActionData> ComboActionData;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill )
	ESkillCollisionShape CollisionShapeType = ESkillCollisionShape::Box;

};
