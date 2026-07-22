// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FragmentedGameplayAbility.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EFragmentedAbilityCostType : uint8
{
	None,
	CloneBuildUp
};

UCLASS()
class FRAGMENTED_API UFragmentedGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:

	UFragmentedGameplayAbility();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	bool ShouldShowInAbilitiesBar = false;

	UFUNCTION(BlueprintCallable, Category = "Helpers")
	bool HasPC() const;

protected:

	// Determines what resource this ability consumes
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cost")
	EFragmentedAbilityCostType CostType = EFragmentedAbilityCostType::None;

	//Amount of resource required
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cost")
	float AbilityCost = 0.f;

	// Gameplay Effect used to modify clone resources
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cost")
	TSubclassOf<UGameplayEffect> CloneCostEffect;
	
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;

	virtual  void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

private:
	// Handles the clone buildup/charge rollover calculation
	void ApplyCloneResourceCost(const FGameplayAbilityActorInfo* ActorInfo) const;
};
