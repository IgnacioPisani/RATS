// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"

#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	Health = 100.f;
	MaxHealth = 100.f;
	MinHealth = 0.f;
	bIsDeath = false;
	// ...
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, Health);
}


void UHealthComponent::UpdateHealth(float HealthPoints)
{
	if (!GetOwner()->HasAuthority()) return;

	Health = FMath::Clamp(Health + HealthPoints, MinHealth, MaxHealth);

	OnLifeChanged.Broadcast(Health, MaxHealth);

	if (Health <= 0.f)
	{
		bIsDeath=true;
		OnDeath.Broadcast();
	}
}


void UHealthComponent::OnRep_Health()
{
	OnLifeChanged.Broadcast(Health, MaxHealth);
}
