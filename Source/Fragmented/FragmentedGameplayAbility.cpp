// Fill out your copyright notice in the Description page of Project Settings.


#include "FragmentedGameplayAbility.h"

#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h"

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

bool UFragmentedGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	if(CostType == EFragmentedAbilityCostType::CloneBuildUp)
	{
		const UBasicAttributeSet* Attributes =
			Cast<UBasicAttributeSet>(
				ActorInfo->AbilitySystemComponent->GetAttributeSet(UBasicAttributeSet::StaticClass())
			);

		if(!Attributes)
		{
			return false;
		}

		return Attributes->GetCloneChargeBuildUp() >= AbilityCost ||  Attributes->GetCloneCharges() > 0;
	}


	return Super::CheckCost(Handle,ActorInfo,OptionalRelevantTags);
}

void UFragmentedGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	if(CostType == EFragmentedAbilityCostType::CloneBuildUp)
	{
		ApplyCloneResourceCost(ActorInfo);
		return;
	}


	Super::ApplyCost(Handle,ActorInfo,ActivationInfo);
}

void UFragmentedGameplayAbility::ApplyCloneResourceCost(const FGameplayAbilityActorInfo* ActorInfo) const
{
	if(!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
	{
		return;
	}

	UAbilitySystemComponent* ASC =
		ActorInfo->AbilitySystemComponent.Get();


	const UBasicAttributeSet* Attributes =
		Cast<UBasicAttributeSet>(
			ASC->GetAttributeSet(UBasicAttributeSet::StaticClass())
		);


	if(!Attributes)
	{
		return;
	}


	float CurrentBuildUp = Attributes->GetCloneChargeBuildUp();
	float CurrentCharges = Attributes->GetCloneCharges();


	float BuildUpDelta;
	float ChargeDelta = 0.f;


	if(CurrentBuildUp >= AbilityCost)
	{
		// Enough buildup, only remove buildup
		BuildUpDelta = -AbilityCost;
	}
	else if(CurrentCharges > 0)
	{
		// Not enough buildup, consume a charge

		float RemainingCost = AbilityCost - CurrentBuildUp;

		ChargeDelta = -1.f;

		// After consuming a charge, remaining resource rolls over
		float NewBuildUp = 100.f - RemainingCost;

		// Convert new total into a delta for the Gameplay Effect
		BuildUpDelta = NewBuildUp - CurrentBuildUp;

		//UE_LOG(LogTemp, Warning, TEXT("Remaining Cost: %f"), RemainingCost);
		//UE_LOG(LogTemp, Warning, TEXT("New Build Up: %f"), NewBuildUp);
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("FAILED COST - No Build Up or Charges"));
		return;
	}

	FGameplayEffectContextHandle ContextHandle =
	ASC->MakeEffectContext();

	ContextHandle.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle = 	ASC->MakeOutgoingSpec(CloneCostEffect,1.f,ContextHandle);

	if(!SpecHandle.IsValid())
	{
		return;
	}

	SpecHandle.Data->SetSetByCallerMagnitude(
	FGameplayTag::RequestGameplayTag("Data.CloneCharge"),
	ChargeDelta);

	SpecHandle.Data->SetSetByCallerMagnitude(
	FGameplayTag::RequestGameplayTag("Data.CloneBuildUp"),
	BuildUpDelta);

	//UE_LOG(LogTemp, Warning, TEXT("Charge Delta: %f"), ChargeDelta);
	//UE_LOG(LogTemp, Warning, TEXT("BuildUp Delta: %f"), BuildUpDelta);

	ASC->ApplyGameplayEffectSpecToSelf(
	*SpecHandle.Data.Get());

}
