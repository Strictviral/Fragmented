// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FragmentedGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class FRAGMENTED_API UFragmentedGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:

	UFragmentedGameplayAbility();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	bool ShouldShowInAbilitiesBar = false;
	
};
