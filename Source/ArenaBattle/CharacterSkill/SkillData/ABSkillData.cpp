// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSkill/SkillData/ABSkillData.h"

UABSkillData::UABSkillData()
{
}

const FCollisionShape UABSkillData::GetCollisionShape(const FVector3f& ExtentModifier, const FVector3f& ExtentMultiplier) const
{
	FCollisionShape CollisionShape;
	FVector3f NewExtent = (SkillExtent + ExtentModifier) * ExtentMultiplier;

	switch (CollisionShapeType)
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
