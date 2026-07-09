// Fill out your copyright notice in the Description page of Project Settings.


#include "FragmentedEnemyCharacterBase.h"

void AFragmentedEnemyCharacterBase::HandleDeath_Implementation()
{
	Super::HandleDeath_Implementation();

	EnemyDeath.Broadcast(this);
}
