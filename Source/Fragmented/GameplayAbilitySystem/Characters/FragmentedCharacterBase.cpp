// Fill out your copyright notice in the Description page of Project Settings.


#include "FragmentedCharacterBase.h"

#include "Components/CapsuleComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "Fragmented/GameplayAbilitySystem/FragmentedAbilitySystemComponent.h"
#include "Fragmented/GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AFragmentedCharacterBase::AFragmentedCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Add ability system component to character
	AbilitySystemComponent = CreateDefaultSubobject<UFragmentedAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(ASCReplicationMode);

	//Create the basic attribute set
	BasicAttributeSet = CreateDefaultSubobject<UBasicAttributeSet>(TEXT("BasicAttributeSet"));
	

	GetCapsuleComponent()->InitCapsuleSize(35.0f, 90.0f);

	//Dont Rotate when controller rotates, Let the camera do that
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	//Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);

	GetCharacterMovement()->JumpZVelocity = 500.0f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag("State.Dead"))
		.AddUObject(this, &AFragmentedCharacterBase::OnDeadTagChanged);

}

// Called when the game starts or when spawned
void AFragmentedCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFragmentedCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitializeAbilitySystemComponent();
	GrantAbilities(StartingAbilities);
	ApplyDefaultAttributes();
}

void AFragmentedCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitializeAbilitySystemComponent();
	ApplyDefaultAttributes();
}

void AFragmentedCharacterBase::OnDeadTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		HandleDeath();
	}
}

void AFragmentedCharacterBase::HandleDeath_Implementation()
{
	//GetMesh()->SetSimulatePhysics((true));
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetCharacterMovement()->DisableMovement();

	//FVector Impulse = GetActorForwardVector() * -200000;
	//Impulse.Z = 15000;
	//GetMesh()->AddImpulseAtLocation(Impulse, GetActorLocation());
}

void AFragmentedCharacterBase::ApplyDefaultAttributes()
{
	//UE_LOG(LogTemp, Warning, TEXT("Initialize attributes"));
	if (!AbilitySystemComponent) return;

	//UE_LOG(LogTemp, Warning, TEXT("Ability system available"));

	if (!DefaultAttributesEffect) return;;

	//UE_LOG(LogTemp, Warning, TEXT("Default Attributes Available"));

	FGameplayEffectContextHandle GameplayEffectHandleContext = AbilitySystemComponent->MakeEffectContext();

	GameplayEffectHandleContext.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributesEffect, 1.f, GameplayEffectHandleContext);

	//UE_LOG(LogTemp, Warning, TEXT("GE Applied? %s"),
	//SpecHandle.IsValid() ? TEXT("YES") : TEXT("NO"));
	
	if (SpecHandle.IsValid())
	{
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		//UE_LOG(LogTemp, Warning, TEXT("Health: %f"),
	//BasicAttributeSet->GetHealth());

		//UE_LOG(LogTemp, Warning, TEXT("Max Health: %f"),
			//BasicAttributeSet->GetMaxHealth());
	}
}

// Called every frame
void AFragmentedCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFragmentedCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UAbilitySystemComponent* AFragmentedCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

TArray<FGameplayAbilitySpecHandle> AFragmentedCharacterBase::GrantAbilities(
	TArray<TSubclassOf<UGameplayAbility>> AbilitiesToGrant)
{
	if (!AbilitySystemComponent || !HasAuthority())
	{
		return TArray<FGameplayAbilitySpecHandle>();
	}

	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;
	
	for (TSubclassOf<UGameplayAbility> Ability : AbilitiesToGrant)
	{
		FGameplayAbilitySpecHandle SpecHandle = AbilitySystemComponent->GiveAbility
		(FGameplayAbilitySpec(Ability,1,-1,this));

		AbilitySpecHandles.Add(SpecHandle);
	}
	SendAbilitiesChanged();
	return AbilitySpecHandles;
}


void AFragmentedCharacterBase::InitializeAbilitySystemComponent()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
}

void AFragmentedCharacterBase::RemoveAbilities(TArray<FGameplayAbilitySpecHandle> AbilityHandleToRemove)
{
	if (!AbilitySystemComponent || !HasAuthority())
	{
		return;
	}

	for (FGameplayAbilitySpecHandle AbilitySpecHandle : AbilityHandleToRemove)
	{
		AbilitySystemComponent->ClearAbility(AbilitySpecHandle);
	}

	SendAbilitiesChanged();
}

void AFragmentedCharacterBase::SendAbilitiesChanged()
{
	FGameplayEventData EventData;
	EventData.EventTag = FGameplayTag::RequestGameplayTag(FName("Event.Abilities.Changed"));
	EventData.Instigator = this;
	EventData.Target = this;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventData.EventTag, EventData);
}

