// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSkill/SkillData/ABSkillData.h"

UABSkillData::UABSkillData()
{
}

const FCollisionShape UABSkillData::GetCollisionShape(const int32 InMotionIndex, const FVector3f& ExtentModifier, const FVector3f& ExtentMultiplier) const
{
	FCollisionShape CollisionShape;
	const FSkillDataPerMotion& NowMotion = GetSkillMotionData(InMotionIndex);
	FVector3f NewExtent = (NowMotion.SkillExtent + ExtentModifier) * ExtentMultiplier;

	switch (NowMotion.CollisionShapeType)
	{
		case ESkillCollisionShape::Box:
		{
			CollisionShape.SetBox(NewExtent);
			break;
		}
		case ESkillCollisionShape::Capsule:
		{
			CollisionShape.SetCapsule(NewExtent);
			break;
		}
		case ESkillCollisionShape::Sphere:
		{
			CollisionShape.SetSphere(NewExtent.GetMax());
			break;
		}
	}

	return CollisionShape;
}

void UABSkillData::PostInitProperties()
{
	Super::PostInitProperties();
	ensureMsgf(MotionDatas.Num() < 0 , TEXT("Check MotionData for %s"),*GetName());
}
