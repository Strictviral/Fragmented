// Fill out your copyright notice in the Description page of Project Settings.


#include "FragmentedEnemyCharacterBase.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BrainComponent.h"

void AFragmentedEnemyCharacterBase::HandleDeath_Implementation()
{
	Super::HandleDeath_Implementation();

	EnemyDeath.Broadcast(this);
}

void AFragmentedEnemyCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	EnemyAIController = Cast<AAIController>(NewController);
}

void AFragmentedEnemyCharacterBase::OnTakenFromPool()
{
	if(EnemyAIController && EnemyAIController->GetBrainComponent())
	{
		EnemyAIController->BrainComponent->RestartLogic();
	}
	GetCapsuleComponent()->SetCollisionEnabled(
		ECollisionEnabled::QueryAndPhysics
	);

	GetCharacterMovement()->SetMovementMode(
		EMovementMode::MOVE_Walking
	);
	ApplyDefaultAttributes();
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
}

void AFragmentedEnemyCharacterBase::OnReturnedToPool()
{
	if(EnemyAIController && EnemyAIController->GetBrainComponent())
	{
		EnemyAIController->BrainComponent->StopLogic(
			"Returned To Pool"
		);
	}
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();
	//ApplyDefaultAttributes();
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
}
