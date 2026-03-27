// Mechanism.cpp
#include "Mechanism.h"

AMechanism::AMechanism()
{
	PrimaryActorTick.bCanEverTick = false;
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	RootComponent = StaticMesh;
}

FItemStruct AMechanism::GetItem_Implementation()
{
	return FItemStruct();
}

void AMechanism::Interact_Implementation(AActor* Interactor)
{
	if (bIsActivated) return;

	bIsActivated = true;
	PlayActivationAnimation();
	OnActivated(Interactor);
}

void AMechanism::OnActivated_Implementation(AActor* Interactor)
{
	// Base no hace nada — subclases sobreescriben
}