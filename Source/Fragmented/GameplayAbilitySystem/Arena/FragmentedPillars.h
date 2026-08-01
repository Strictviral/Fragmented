// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Fragmented/GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "FragmentedPillars.generated.h"

class AFragmentedArenaManager;
class AFragmentedPillars;
class UGameplayEffect;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPillarHalfHealth, AFragmentedPillars*, FragmentedPillar);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPillarActivated,AFragmentedPillars*, Pillar);

UCLASS()
class FRAGMENTED_API AFragmentedPillars : public AActor , public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFragmentedPillars();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameplayAbilitySystem")
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	class UBasicAttributeSet* BasicAttributeSet;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category="GAS")
	TSubclassOf<UGameplayEffect> DefaultAttributesEffect;

	UPROPERTY()
	bool bPillarActivated = false;

	UPROPERTY()
    bool bHalfHealthTriggered = false;

protected:

	UFUNCTION()
	void InitializeAttributes();
	
    void OnHealthChanged(const FOnAttributeChangeData& Data);

	UFUNCTION()
	void InitializeAbilitySystemComponent();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable, Category = "Pillar|Events")
	FOnPillarHalfHealth OnHalfHealthReached;

	UPROPERTY(BlueprintAssignable, Category = "Pillar|Events")
	FOnPillarActivated OnPillarActivated;

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

};
