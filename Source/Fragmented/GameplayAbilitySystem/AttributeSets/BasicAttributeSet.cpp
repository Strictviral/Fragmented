// Fill out your copyright notice in the Description page of Project Settings.


#include "Fragmented/GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UBasicAttributeSet::UBasicAttributeSet()
{
	Health = 100.f;
	MaxHealth = 100.f;
	Stamina = 100.f;
	MaxStamina = 100.f;
	CloneCharges = 3.f;
	CloneChargeBuildUp = 0.f;
}

void UBasicAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, CloneCharges, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, CloneChargeBuildUp, COND_None, REPNOTIFY_Always);
	
}

void UBasicAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		//NewValue = FMath::Clamp(NewValue,0.f, GetMaxHealth());
	}else if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue,0.f, GetMaxStamina());
	}else if (Attribute == GetCloneChargesAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, 3.0f);
	}else if (Attribute == GetCloneChargeBuildUpAttribute())
	{
		NewValue = FMath::Clamp(NewValue,0.f, 999.f);
	}
}

void UBasicAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetHealthAttribute() && NewValue <= 0.f)
	{
		FGameplayTagContainer DeathAbilityTagContainer;
		DeathAbilityTagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("GameplayAbility.Death")));
		GetOwningAbilitySystemComponent()->TryActivateAbilitiesByTag(DeathAbilityTagContainer);
	}
}

void UBasicAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(GetHealth());

		if (Data.EffectSpec.Def->GetAssetTags().HasTag(FGameplayTag::RequestGameplayTag("Effects.HitReaction"))
			&& Data.EvaluatedData.Magnitude != 0)
		{
			FGameplayTagContainer HitReactionTagContainer;
			HitReactionTagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("GameplayAbility.HitReaction")));
			GetOwningAbilitySystemComponent()->TryActivateAbilitiesByTag(HitReactionTagContainer);
		}
	}else if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(GetStamina());
	}

	else if (Data.EvaluatedData.Attribute == GetCloneChargeBuildUpAttribute())
	{
		float CurrentBuildup = GetCloneChargeBuildUp();

		if (CurrentBuildup >= 100.f)
		{
			int32 ChargesEarned = FMath::FloorToInt(CurrentBuildup/100.f);

			float RemainingBuildUp = FMath::Fmod(CurrentBuildup, 100.f);

			SetCloneChargeBuildUp(RemainingBuildUp);

			SetCloneCharges(FMath::Clamp(GetCloneCharges()+ ChargesEarned, 0.f, 3.f));
			
		}
	}
}
