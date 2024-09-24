// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_AttackHitCheck.h"
#include "CharacterSkill/ABCharacterSkillComponent.h"

void UAnimNotify_AttackHitCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		UABCharacterSkillComponent* SkillComponent = MeshComp->GetOwner()->GetComponentByClass<UABCharacterSkillComponent>();

		if (SkillComponent)
		{
			SkillComponent->PerformSkillHitCheck();
		}
	}
}
