// Fill out your copyright notice in the Description page of Project Settings.


#include "FragmentedGameplayAbility.h"

#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"

UFragmentedGameplayAbility::UFragmentedGameplayAbility()
{
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("GameplayAbility.Active")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag("State.Dead"));
}

bool UFragmentedGameplayAbility::HasPC() const
{
	const APawn* PawnObject = Cast<APawn>(GetAvatarActorFromActorInfo());
	if (!PawnObject)
	{
		return false;
	}

	return PawnObject->GetController()->IsA<APlayerController>();
}
