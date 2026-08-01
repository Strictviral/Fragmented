// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyPoolManager.h"
#include "FragmentedPillars.h"
#include "Fragmented/GameplayAbilitySystem/Characters/FragmentedEnemyCharacterBase.h"
#include "GameFramework/Actor.h"
#include "WaveData.h"
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

	//Spawning Class
	UPROPERTY(EditDefaultsOnly, Category="Spawning")
	TSubclassOf<AFragmentedEnemyCharacterBase> MeleeEnemyClass;

	UPROPERTY(EditDefaultsOnly, Category="Spawning")
	TSubclassOf<AFragmentedEnemyCharacterBase> RangedEnemyClass;

	UPROPERTY(EditDefaultsOnly, Category="Spawning")
	TSubclassOf<AFragmentedEnemyCharacterBase> RedSpecialEnemyClass;

	UPROPERTY(EditDefaultsOnly, Category="Spawning")
	TSubclassOf<AFragmentedEnemyCharacterBase> GreenSpecialEnemyClass;
	
	//Map to Show which enemies belong to which crystal
	UPROPERTY()
	TMap<AFragmentedPillars*, FEnemyArrayWrapper> ActiveEnemies;

	//Wave Data
	UPROPERTY(EditDefaultsOnly, Category = "Waves")
	UDataTable* WaveDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waves")
	FName CurrentWaveRow;

	//Enemy PoolManager Reference
	UPROPERTY(EditInstanceOnly, Category="Pooling")
	AEnemyPoolManager* EnemyPoolManager;

private:

	


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	
	UFUNCTION()
	void OnPillarActivated(AFragmentedPillars* Pillar);
	
	UFUNCTION()
	void InitializePillars();

	bool GetWaveData(FName WaveRow, FWaveData& OutWaveData) const;

	void SpawnEnemyGroup(AFragmentedPillars* Pillar,TSubclassOf<AFragmentedEnemyCharacterBase> EnemyToSpawn,int32 Amount);


private:

	UFUNCTION()
	FVector GetRandomSpawnPoint(AFragmentedPillars* Pillar);
	
	UFUNCTION()
	void SpawnWaveForPillar(AFragmentedPillars* Pillar, EWaveType WaveType);

	UFUNCTION()
	void OnPillarHalfHealth(AFragmentedPillars* Pillar);

	UFUNCTION()
	void HandleEnemyDeath(AFragmentedEnemyCharacterBase* DeadEnemy);

	UFUNCTION()
	void ActivateBoss();
	
	
};
