// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FragmentedCharacterBase.h"

#include "Fragmented/GameplayAbilitySystem/Arena/FragmentedPillars.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "FragmentedEnemyCharacterBase.generated.h"

/**
 * 
 */
class AFragmentedEnemyCharacterBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDeath, AFragmentedEnemyCharacterBase*, DeadEnemy);

UCLASS()
class FRAGMENTED_API AFragmentedEnemyCharacterBase : public AFragmentedCharacterBase
{
	GENERATED_BODY()

public:

	UPROPERTY()
	AFragmentedPillars* OwningPillar;

	UPROPERTY(BlueprintAssignable, Category = "Pillar|Events")
	FOnEnemyDeath EnemyDeath;

	UPROPERTY()
	TSubclassOf<AFragmentedEnemyCharacterBase> EnemyType;
	
	virtual void OnTakenFromPool();

	virtual void OnReturnedToPool();

protected:

	virtual void HandleDeath_Implementation() override;

	virtual void PossessedBy(AController* NewController) override;

	UPROPERTY()
	AAIController* EnemyAIController;

	
};
