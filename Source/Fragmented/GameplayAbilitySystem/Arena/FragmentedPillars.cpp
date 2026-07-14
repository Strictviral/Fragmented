// Fill out your copyright notice in the Description page of Project Settings.


#include "FragmentedPillars.h"

#include "Fragmented/GameplayAbilitySystem/FragmentedAbilitySystemComponent.h"

// Sets default values
AFragmentedPillars::AFragmentedPillars()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UFragmentedAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	BasicAttributeSet = CreateDefaultSubobject<UBasicAttributeSet>(TEXT("Attribite Set"));

	UE_LOG(LogTemp, Warning, TEXT("Constructor Fired"));
}

// Called when the game starts or when spawned
void AFragmentedPillars::BeginPlay()
{
	Super::BeginPlay();

	InitializeAbilitySystemComponent();
	
	//UE_LOG(LogTemp, Warning, TEXT("BeginPlay"));
	
	InitializeAttributes();
	

	AbilitySystemComponent
		->GetGameplayAttributeValueChangeDelegate(
			UBasicAttributeSet::GetHealthAttribute()
		)
		.AddUObject(
			this,
			&AFragmentedPillars::OnHealthChanged
		);
}

void AFragmentedPillars::InitializeAttributes()
{
	UE_LOG(LogTemp, Warning, TEXT("Initialize attributes"));
	if (!AbilitySystemComponent) return;

	UE_LOG(LogTemp, Warning, TEXT("Ability system available"));

	if (!DefaultAttributesEffect) return;;

	UE_LOG(LogTemp, Warning, TEXT("Default Attributes Available"));

	FGameplayEffectContextHandle GameplayEffectHandleContext = AbilitySystemComponent->MakeEffectContext();

	GameplayEffectHandleContext.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributesEffect, 1.f, GameplayEffectHandleContext);

	UE_LOG(LogTemp, Warning, TEXT("GE Applied? %s"),
	SpecHandle.IsValid() ? TEXT("YES") : TEXT("NO"));
	
	if (SpecHandle.IsValid())
	{
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		UE_LOG(LogTemp, Warning, TEXT("Health: %f"),
	BasicAttributeSet->GetHealth());

		UE_LOG(LogTemp, Warning, TEXT("Max Health: %f"),
			BasicAttributeSet->GetMaxHealth());
	}
}

void AFragmentedPillars::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if (bHalfHealthTriggered == true) return;

	float CurrentHealth = Data.NewValue;

	float MaxHealth =
		Cast<UBasicAttributeSet>(BasicAttributeSet)->GetMaxHealth();

	if (CurrentHealth <= MaxHealth * 0.5f)
	{
		bHalfHealthTriggered = true;

		AbilitySystemComponent->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Invulnerable")));

		FGameplayTagContainer OwnedTags;
		AbilitySystemComponent->GetOwnedGameplayTags(OwnedTags);

		//UE_LOG(LogTemp, Warning, TEXT("=== Owned Tags ==="));

		/*for (const FGameplayTag& Tag : OwnedTags)
		{
			//UE_LOG(LogTemp, Warning, TEXT("%s"), *Tag.ToString());
		}*/

		OnHalfHealthReached.Broadcast(this);
	}
}

void AFragmentedPillars::InitializeAbilitySystemComponent()
{
	if (AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Ability Initalized"));
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
}

// Called every frame
void AFragmentedPillars::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UAbilitySystemComponent* AFragmentedPillars::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

