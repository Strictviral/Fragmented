// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Fragmented/GameplayAbilitySystem/Characters/FragmentedEnemyCharacterBase.h"
#include "GameFramework/Actor.h"
#include "EnemyPoolManager.generated.h"

USTRUCT()
struct FEnemyPoolWrapper
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<AFragmentedEnemyCharacterBase*> Enemies;
};

UCLASS()
class FRAGMENTED_API AEnemyPoolManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyPoolManager();


	UFUNCTION()
	AFragmentedEnemyCharacterBase* RequestEnemy(TSubclassOf<AFragmentedEnemyCharacterBase> EnemyClass);
	
	UFUNCTION()
	void ReturnEnemy(AFragmentedEnemyCharacterBase* Enemy,TSubclassOf<AFragmentedEnemyCharacterBase> EnemyClass);


protected:

	virtual void BeginPlay() override;


	//Map to Have an array of enemies attached to a class
	UPROPERTY()
	TMap<TSubclassOf<AFragmentedEnemyCharacterBase>,FEnemyPoolWrapper> EnemyPools;
	
	void CreateEnemyPool(TSubclassOf<AFragmentedEnemyCharacterBase> EnemyClass,int32 PoolSize);

	//Set the Enemy Types and Number associated with them - Instructions to fill in CreateEnemyPool Function 
	UPROPERTY(EditDefaultsOnly, Category="Enemy Pool")
	TMap<TSubclassOf<AFragmentedEnemyCharacterBase>,int32> PoolSizesWithEnemyClasses;


	


public:

	virtual void Tick(float DeltaTime) override;

};
