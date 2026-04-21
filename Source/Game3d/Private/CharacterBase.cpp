// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"

#include "HealthComponent.h"

// Sets default values
ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	if (HealthComponent)
	{
		HealthComponent->OnLifeChanged.AddDynamic(this, &ACharacterBase::HandleLifeChanged);
		HealthComponent->OnDeath.AddDynamic(this, &ACharacterBase::HandleDeath);
	}
}

float ACharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (HealthComponent)
	{
		HealthComponent->UpdateHealth(DamageAmount);
		TakeDamageEffects();
		UE_LOG(LogTemp, Warning, TEXT("Danio"));

	}
	return DamageAmount;
}

void ACharacterBase::HandleLifeChanged(float Health, float MaxHealth)
{

}

void ACharacterBase::HandleHit_Implementation()
{
	// Puede ser sobrescrito por cada hijo si necesitan lógica adicional
}

void ACharacterBase::HandleDeath()
{
	Destroy();
}

void ACharacterBase::TakeDamageEffects()
{
	
}

// Called every frame
void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

