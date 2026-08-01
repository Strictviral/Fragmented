// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyPoolManager.h"

#include "Engine/World.h"

// Sets default values
AEnemyPoolManager::AEnemyPoolManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemyPoolManager::BeginPlay()
{
	Super::BeginPlay();

	// Searches through our enemy class map dynamically
	for(auto& PoolEntry : PoolSizesWithEnemyClasses)
	{
		CreateEnemyPool(
			PoolEntry.Key,
			PoolEntry.Value
		);
	}
}


AFragmentedEnemyCharacterBase* AEnemyPoolManager::RequestEnemy(TSubclassOf<AFragmentedEnemyCharacterBase> EnemyType)
{
	if(!EnemyType)
	{
		return nullptr;
	}

	// Get the wrapper that contains the enemy array
	FEnemyPoolWrapper* PoolWrapper = EnemyPools.Find(EnemyType);

	if(!PoolWrapper || PoolWrapper->Enemies.Num() == 0)
	{
		return nullptr;
	}

	// Get first available enemy
	AFragmentedEnemyCharacterBase* Enemy = PoolWrapper->Enemies[0];
	
	// Remove from inactive pool
	PoolWrapper->Enemies.RemoveAt(0);
	
	// Activate enemy
	Enemy->OnTakenFromPool();
	
	return Enemy;
}


void AEnemyPoolManager::ReturnEnemy(AFragmentedEnemyCharacterBase* Enemy,TSubclassOf<AFragmentedEnemyCharacterBase> EnemyType)
{
	if(!Enemy || !EnemyType)
	{
		return;
	}
	
	// Find or create pool wrapper
	FEnemyPoolWrapper& PoolWrapper = EnemyPools.FindOrAdd(EnemyType);
	
	// Disable enemy
	Enemy->OnReturnedToPool();

	// Move back to pool location
	Enemy->SetActorLocation(GetActorLocation());
	
	// Add back into inactive pool
	PoolWrapper.Enemies.Add(Enemy);
}


void AEnemyPoolManager::CreateEnemyPool(TSubclassOf<AFragmentedEnemyCharacterBase> EnemyClass,int32 PoolSize)
{
	if(!EnemyClass)
	{
		return;
	}
	
	// Get the wrapper for this enemy type
	FEnemyPoolWrapper& PoolWrapper = EnemyPools.FindOrAdd(EnemyClass);
	
	for(int32 i = 0; i < PoolSize; i++)
	{
		AFragmentedEnemyCharacterBase* Enemy = GetWorld()->SpawnActor<AFragmentedEnemyCharacterBase>(EnemyClass,GetActorLocation(),FRotator::ZeroRotator);
		
		if(!Enemy)
		{
			continue;
		}
		
		// Store what pool this enemy belongs to
		Enemy->EnemyType = EnemyClass;
		
		// Put enemy into inactive state
		Enemy->OnReturnedToPool();
		
		// Add to pool
		PoolWrapper.Enemies.Add(Enemy);
	}
}


// Called every frame
void AEnemyPoolManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

