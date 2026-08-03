// Fill out your copyright notice in the Description page of Project Settings.


#include "FragmentedArenaManager.h"
#include "NavigationSystem.h"
#include "BrainComponent.h"
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
		//UE_LOG(LogTemp, Warning, TEXT("Got Pillars"));

		//Ensure map entry exists
		ActiveEnemies.Add(Pillar, FEnemyArrayWrapper());

		// Bind event
		Pillar->OnHalfHealthReached.AddDynamic(this,&AFragmentedArenaManager::OnPillarHalfHealth);

		Pillar->OnPillarActivated.AddDynamic(this,&AFragmentedArenaManager::OnPillarActivated);
		//UE_LOG(LogTemp, Warning, TEXT("Binded to Pillars half health"));
	}
}

bool AFragmentedArenaManager::GetWaveData(FName WaveRow,FWaveData& OutWaveData) const
{
	if(!WaveDataTable) return false;

	//Find the Row name and tore it
	const FWaveData* Wave = WaveDataTable->FindRow<FWaveData>(WaveRow,TEXT("Wave Lookup"));
	
	if(!Wave) return false;

	//Modify the variable that we passed in
	OutWaveData = *Wave;

	return true;
}

void AFragmentedArenaManager::ReturnEnemyToPool(AFragmentedEnemyCharacterBase* DeadEnemy)
{

	if(!DeadEnemy) return;

	DeadEnemy->EnemyDeath.RemoveAll(this);
	DeadEnemy->EnemyDeathFinished.RemoveAll(this);

	EnemyPoolManager->ReturnEnemy(DeadEnemy,DeadEnemy->EnemyType);
}

void AFragmentedArenaManager::SpawnEnemyGroup(AFragmentedPillars* Pillar,TSubclassOf<AFragmentedEnemyCharacterBase> EnemyToSpawn, int32 Amount)
{

	if (!Pillar || !EnemyToSpawn || Amount <= 0)
	{
		return;
	}

	//Prepare the Enemy Wrapper 
	FEnemyArrayWrapper& ActiveEnemyArrayWrapper = ActiveEnemies[Pillar];
	
	for (int32 i = 0; i < Amount; i++)
	{
		FVector SpawnLocation = GetRandomSpawnPoint(Pillar);

		if (SpawnLocation.Equals(Pillar->GetActorLocation()))
		{
			//UE_LOG(LogTemp, Warning, TEXT("StopSpawning Enemy"));
			continue;
		}
		
		AFragmentedEnemyCharacterBase* Enemy = EnemyPoolManager->RequestEnemy(EnemyToSpawn);
		if(!Enemy) continue;

		Enemy->SetActorLocation(SpawnLocation);
		//link enemy to Crystal
		Enemy->OwningPillar = Pillar;

		// bind death
		Enemy->EnemyDeath.AddDynamic(this, &AFragmentedArenaManager::HandleEnemyDeath);
		Enemy->EnemyDeathFinished.AddDynamic(this, &AFragmentedArenaManager::ReturnEnemyToPool);

		ActiveEnemyArrayWrapper.Enemies.Add(Enemy);
	}
}


void AFragmentedArenaManager::SpawnWaveForPillar(AFragmentedPillars* Pillar,EWaveType WaveType)
{
	if(!Pillar || !WaveDataTable) return;
	//UE_LOG(LogTemp, Warning, TEXT("Pillar Found"));

	//Get the array of Names from the Data Table
	TArray<FName> AvailableWaves;


	for (const FName& RowName : WaveDataTable->GetRowNames())
	{
		const FWaveData* Wave =	WaveDataTable->FindRow<FWaveData>(RowName,TEXT("Wave Lookup"));

		if(Wave && Wave->WaveType == WaveType)
		{
			AvailableWaves.Add(RowName);
		}
	}


	if(AvailableWaves.Num() == 0) return;
	
	int32 RandomIndex =	FMath::RandRange(0,AvailableWaves.Num() - 1);

	//Select random name from array 
	FName SelectedWave = AvailableWaves[RandomIndex];
	
	FWaveData Wave;
	
	if(!GetWaveData(SelectedWave, Wave))	return;
	
	SpawnEnemyGroup(Pillar, MeleeEnemyClass, Wave.MeleeCount);
	SpawnEnemyGroup(Pillar, RangedEnemyClass, Wave.RangedCount);
	SpawnEnemyGroup(Pillar, GreenSpecialEnemyClass, Wave.GreenSpecialCount);
	SpawnEnemyGroup(Pillar, RedSpecialEnemyClass, Wave.RedSpecialCount);

	UE_LOG(LogTemp, Warning,
	TEXT("Spawning Wave: Melee %d | Ranged %d | Green %d | Red %d"),
	Wave.MeleeCount,
	Wave.RangedCount,
	Wave.GreenSpecialCount,
	Wave.RedSpecialCount);
}

void AFragmentedArenaManager::OnPillarActivated(AFragmentedPillars* Pillar)
{
	if(!Pillar) return;

	UE_LOG(LogTemp, Warning, TEXT("Pillar Activated"));

	SpawnWaveForPillar(Pillar, EWaveType::Initial);
}

void AFragmentedArenaManager::OnPillarHalfHealth(AFragmentedPillars* Pillar)
{
	if (!Pillar) return;

	UE_LOG(LogTemp, Warning, TEXT("Got The Dispatcher From Pillar halfHealth"));
	SpawnWaveForPillar(Pillar, EWaveType::HalfHealth);
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
	//DeadEnemy->EnemyDeath.RemoveAll(this);
	//EnemyPoolManager->ReturnEnemy(DeadEnemy, DeadEnemy->EnemyType);


	if (EnemyArrayWrapper->Enemies.Num() == 0)
	{
		DeadEnemy->OwningPillar->AbilitySystemComponent->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Invulnerable")));
	}
}

void AFragmentedArenaManager::ActivateBoss()
{
	if(!BossEnemy) return;

	AAIController* AIController =  Cast<AAIController>(BossEnemy->GetController());

	if(AIController && AIController->GetBrainComponent())
	{
		AIController->GetBrainComponent()->RestartLogic();
	}
}

FVector AFragmentedArenaManager::GetRandomSpawnPoint(AFragmentedPillars* Pillar)
{
	if (!Pillar) return  FVector::ZeroVector;

	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

	if (!NavSystem)
	{
		//UE_LOG(LogTemp, Warning, TEXT("No NavSystem Detected"));
		return Pillar->GetActorLocation();
		
	}

	// Try and find a point on the navMesh
	for (int32 Attempt = 0; Attempt < 10; Attempt++)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Trying to Find Point on navMesh"));
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
				//UE_LOG(LogTemp, Warning, TEXT("Point Found"));
				return NavLocation.Location;
			}
		}
	}
	return FVector::ZeroVector;

	
}



