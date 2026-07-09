// Fill out your copyright notice in the Description page of Project Settings.


#include "FragmentedArenaManager.h"
#include "NavigationSystem.h"
#include "Engine/World.h"

// Sets default values
AFragmentedArenaManager::AFragmentedArenaManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFragmentedArenaManager::BeginPlay()
{
	Super::BeginPlay();

	InitializePillars();
	
}

// Called every frame
void AFragmentedArenaManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFragmentedArenaManager::InitializePillars()
{
	for (AFragmentedPillars* Pillar : Pillars)
	{
		if (!Pillar) continue;
		UE_LOG(LogTemp, Warning, TEXT("Got Pillars"));

		//Ensure map entry exists
		ActiveEnemies.Add(Pillar, FEnemyArrayWrapper());

		// Bind event
		Pillar->OnHalfHealthReached.AddDynamic(
			this,
			&AFragmentedArenaManager::OnPillarHalfHealth
		);
		UE_LOG(LogTemp, Warning, TEXT("Binded to Pillars half health"));
	}
}

FVector AFragmentedArenaManager::GetRandomSpawnPoint(AFragmentedPillars* Pillar)
{
	if (!Pillar) return  FVector::ZeroVector;

	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

	if (!NavSystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("No NavSystem Detected"));
		return Pillar->GetActorLocation();
		
	}

	// Try and find a point on the navMesh
	for (int32 Attempt = 0; Attempt < 10; Attempt++)
	{
		UE_LOG(LogTemp, Warning, TEXT("Trying to Find Point on navMesh"));
		FNavLocation NavLocation;

		if (NavSystem->GetRandomReachablePointInRadius(
			Pillar->GetActorLocation(),
			MaxSpawnRadius,
			NavLocation))
		{
			float Distance =
				FVector::Dist2D(
					Pillar->GetActorLocation(),
					NavLocation.Location);

			if (Distance >= MinSpawnRadius)
			{
				UE_LOG(LogTemp, Warning, TEXT("Point Found"));
				return NavLocation.Location;
			}
		}
	}
	return FVector::ZeroVector;

	/*FVector Origin = Pillar->GetActorLocation();

	float Radius = FMath::FRandRange(MinSpawnRadius, MaxSpawnRadius);

	FVector RandomDirection = FMath::VRand();
	RandomDirection.Z = 0.f;
	RandomDirection.Normalize();

	return Origin + (RandomDirection * Radius);*/
}

void AFragmentedArenaManager::SpawnWaveForPillar(AFragmentedPillars* Pillar)
{
	if(!Pillar) return;
	UE_LOG(LogTemp, Warning, TEXT("Pillar Found"));
	if(!EnemyClass) return;
	UE_LOG(LogTemp, Warning, TEXT("EnemyClassFound"));

	//This gets the Array associated with the Pillar, then get and stores a reference to the original array 
	FEnemyArrayWrapper& EnemyArrayWrapper = ActiveEnemies[Pillar];

	int32 WaveSize = 5;

	for (int32 i = 0; i < WaveSize; i++)
	{
		FVector SpawnLocation = GetRandomSpawnPoint(Pillar);

		if (SpawnLocation.Equals(Pillar->GetActorLocation()))
		{
			UE_LOG(LogTemp, Warning, TEXT("StopSpawning Enemy"));
			continue;
		}

		AFragmentedEnemyCharacterBase* Enemy =
			GetWorld()->SpawnActor<AFragmentedEnemyCharacterBase>(
				EnemyClass,
				SpawnLocation,
				FRotator::ZeroRotator
				);
		if(!Enemy) continue;

		//link enemy to Crystal
		Enemy->OwningPillar = Pillar;

		// bind death
		Enemy->EnemyDeath.AddDynamic(this, &AFragmentedArenaManager::HandleEnemyDeath);

		EnemyArrayWrapper.Enemies.Add(Enemy);
	}
}
void AFragmentedArenaManager::OnPillarHalfHealth(AFragmentedPillars* Pillar)
{
	if (!Pillar) return;

	UE_LOG(LogTemp, Warning, TEXT("Got The Dispatcher From Pillar halfHealth"));
	SpawnWaveForPillar(Pillar);
}

void AFragmentedArenaManager::HandleEnemyDeath(AFragmentedEnemyCharacterBase* DeadEnemy)
{
	if (!DeadEnemy) return;

	AFragmentedPillars* Pillar = DeadEnemy->OwningPillar;
	if (!Pillar) return;

	//Gets the address of the array
	FEnemyArrayWrapper* EnemyArrayWrapper = ActiveEnemies.Find(Pillar);
	if (!EnemyArrayWrapper) return;

	EnemyArrayWrapper->Enemies.Remove(DeadEnemy);

	DeadEnemy->EnemyDeath.RemoveAll(this);

	if (EnemyArrayWrapper->Enemies.Num() == 0)
	{
		DeadEnemy->OwningPillar->AbilitySystemComponent->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Invulnerable")));
	}
}

void AFragmentedArenaManager::ActivateBoss()
{
	//Send Delegate to Boss to Activate
}


