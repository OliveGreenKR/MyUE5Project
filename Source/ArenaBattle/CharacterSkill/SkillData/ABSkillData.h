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

USTRUCT(BlueprintType)
struct FSkillDataPerMotion
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill, meta = (ToolTip = "Forward, Width, Height"))
	FVector3f SkillExtent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill, meta = (ToolTip = "-1 is just Frame"))
	float SkillEffectiveTime = -1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill)
	float SkillRawDamage = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill)
	ESkillCollisionShape CollisionShapeType = ESkillCollisionShape::Box;
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

	const struct FCollisionShape GetCollisionShape(const int32 InMotionIndex = 0 , const FVector3f& ExtentModifier = FVector3f::ZeroVector , 
												   const FVector3f& ExtentMultiplier =  FVector3f::OneVector) const;
public:
	FORCEINLINE const FSkillDataPerMotion& GetSkillMotionData(const int32 InMotionIndex) const
	{
		return MotionDatas.IsValidIndex(InMotionIndex) ? MotionDatas[InMotionIndex] : MotionDatas[0];
	}

protected:
	virtual void PostInitProperties() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill)
	FString SkillName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill)
	float SkillCoolTime = -1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill)
	TArray<FSkillDataPerMotion> MotionDatas;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill)
	TObjectPtr<class UAnimMontage> SkillMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UABComboActionData> ComboActionData;
};
