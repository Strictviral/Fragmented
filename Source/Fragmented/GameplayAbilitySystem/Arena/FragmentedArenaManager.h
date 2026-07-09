// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FragmentedPillars.h"
#include "Fragmented/GameplayAbilitySystem/Characters/FragmentedEnemyCharacterBase.h"
#include "GameFramework/Actor.h"
#include "FragmentedArenaManager.generated.h"


USTRUCT()
struct FEnemyArrayWrapper
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<AFragmentedEnemyCharacterBase*> Enemies;
};

UCLASS()
class FRAGMENTED_API AFragmentedArenaManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFragmentedArenaManager();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Holds a List of All Pillars
	UPROPERTY(EditInstanceOnly, Category = "Arena")
	TArray<AFragmentedPillars*> Pillars;

	//Spawning Radius Settings
	UPROPERTY(EditAnywhere, Category = "Waves")
	float MinSpawnRadius = 3000.f;

	UPROPERTY(EditAnywhere, Category = "Waves")
	float MaxSpawnRadius = 6000.f;


private:

	


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	UFUNCTION()
	void InitializePillars();

	UPROPERTY(EditDefaultsOnly, Category="Spawning")
	TSubclassOf<AFragmentedEnemyCharacterBase> EnemyClass;

	//Map to Show which enemies belong to which crystal
	UPROPERTY()
	TMap<AFragmentedPillars*, FEnemyArrayWrapper> ActiveEnemies;

	

private:

	UFUNCTION()
	FVector GetRandomSpawnPoint(AFragmentedPillars* Pillar);
	
	UFUNCTION()
	void SpawnWaveForPillar(AFragmentedPillars* Pillar);

	UFUNCTION()
	void OnPillarHalfHealth(AFragmentedPillars* Pillar);

	UFUNCTION()
	void HandleEnemyDeath(AFragmentedEnemyCharacterBase* DeadEnemy);

	UFUNCTION()
	void ActivateBoss();
	
	
};
