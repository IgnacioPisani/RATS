// Fill out your copyright notice in the Description page of Project Settings.


#include "XpComponent.h"

// Sets default values for this component's properties
UXpComponent::UXpComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	 Xp=0;	
	 MaxXp = 100;
	 Level = 1;
}


// Called when the game starts
void UXpComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UXpComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UXpComponent::IncreaseLevel(float AddedLevel)
{
	Level = AddedLevel + Level;
	MaxXp = MaxXp + 150.f;
	Xp = 0.f;
	OnLevelChanged.Broadcast(Level);
	OnXpChanged.Broadcast(Xp, MaxXp);
}

void UXpComponent::IncreaseXp(float XpPoints)
{
	Xp = XpPoints + Xp;
	OnXpChanged.Broadcast(Xp, MaxXp);
	if (Xp >= MaxXp) {
		IncreaseLevel(1);
	}
}

