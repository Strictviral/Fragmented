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
	ActivateEnemy();
}

void AFragmentedEnemyCharacterBase::OnReturnedToPool()
{
	DeactivateEnemy();
}

void AFragmentedEnemyCharacterBase::ActivateEnemy()
{
	if(EnemyAIController && EnemyAIController->GetBrainComponent())
	{
		EnemyAIController->BrainComponent->RestartLogic();
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	ApplyDefaultAttributes();
	if(AbilitySystemComponent)
	{
		AbilitySystemComponent->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("State.Active"));
	}
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
}

void AFragmentedEnemyCharacterBase::DeactivateEnemy()
{
	if(EnemyAIController && EnemyAIController->GetBrainComponent())
	{
		EnemyAIController->BrainComponent->StopLogic("Returned To Pool");
	}
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();
	//ApplyDefaultAttributes();
	SetActorHiddenInGame(true);
	if(AbilitySystemComponent)
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("State.Active"));
	}
	SetActorEnableCollision(false);
}
