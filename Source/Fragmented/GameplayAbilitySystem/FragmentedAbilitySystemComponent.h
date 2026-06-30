// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "FragmentedAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class FRAGMENTED_API UFragmentedAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

	UFragmentedAbilitySystemComponent();

protected:

	void OnRep_ActivateAbilities() override;

protected:

	TArray<FGameplayAbilitySpec> LastActivatableAbilities;
	
};
