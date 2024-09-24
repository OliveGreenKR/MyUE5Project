// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSkill/SkillData/ABSkillData.h"

UABSkillData::UABSkillData()
{
}

const FCollisionShape UABSkillData::GetCollisionShape() const
{
	FCollisionShape CollisionShape;

	switch (CollisionShapeType)
	{
		case ESkillCollisionShape::Box:
		{
			CollisionShape.SetBox(SkillExtent);
			break;
		}
		case ESkillCollisionShape::Capsule:
		{
			CollisionShape.SetCapsule(SkillExtent);
			break;
		}
		case ESkillCollisionShape::Sphere:
		{
			CollisionShape.SetSphere(SkillExtent.GetMax());
			break;
		}
	}

	return CollisionShape;
}
