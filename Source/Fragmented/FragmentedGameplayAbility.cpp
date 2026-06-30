// Fill out your copyright notice in the Description page of Project Settings.


#include "FragmentedGameplayAbility.h"

UFragmentedGameplayAbility::UFragmentedGameplayAbility()
{
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("GameplayAbility.Active")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag("State.Dead"));
}
