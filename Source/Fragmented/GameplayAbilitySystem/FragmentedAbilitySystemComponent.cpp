// Fill out your copyright notice in the Description page of Project Settings.


#include "Fragmented/GameplayAbilitySystem/FragmentedAbilitySystemComponent.h"

#include "Characters/FragmentedCharacterBase.h"

UFragmentedAbilitySystemComponent::UFragmentedAbilitySystemComponent()
{
	
}

void UFragmentedAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	AFragmentedCharacterBase *Character = Cast<AFragmentedCharacterBase>(GetOwner());

	if (!Character)
	{
		return;
	}
	
	bool bAbilitiesChanged = false;
	if (LastActivatableAbilities.Num() != ActivatableAbilities.Items.Num())
	{
		bAbilitiesChanged = true;
	}
	else
	{
		for (int32 i = 0; i < LastActivatableAbilities.Num(); i++)
		{
			if (LastActivatableAbilities[i].Ability != ActivatableAbilities.Items[i].Ability)
			{
				bAbilitiesChanged = true;
				break;
			}
		}
	}

	if (bAbilitiesChanged)
	{
		Character->SendAbilitiesChanged();
		LastActivatableAbilities = ActivatableAbilities.Items;
	}
}
