// Mechanism.cpp
#include "Mechanism.h"
#include "Net/UnrealNetwork.h"

AMechanism::AMechanism()
{
	PrimaryActorTick.bCanEverTick = false;

	// Habilitar replicación del actor
	bReplicates = true;
	bAlwaysRelevant = false; // Usar distancia de relevancia por defecto

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	RootComponent = StaticMesh;
}

void AMechanism::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMechanism, bIsActivated);
}

FItemStruct AMechanism::GetItem_Implementation()
{
	return FItemStruct();
}

void AMechanism::Interact_Implementation(AActor* Interactor)
{
	// Solo el servidor ejecuta la lógica
	if (!HasAuthority()) return;
	if (bIsActivated) return;

	bIsActivated = true;

	// Notificar a todos los clientes para efectos visuales/sonoros
	Multicast_PlayActivationEffects();

	OnActivated(Interactor);
}

void AMechanism::Multicast_PlayActivationEffects_Implementation()
{
	// Se ejecuta en server y en todos los clientes
	PlayActivationAnimation();
}

void AMechanism::OnRep_IsActivated()
{
	// Llamado automáticamente en clientes cuando bIsActivated cambia
	// Útil si un cliente se conecta tarde y necesita sincronizar estado visual
	if (bIsActivated)
	{
		PlayActivationAnimation();
	}
}

void AMechanism::OnActivated_Implementation(AActor* Interactor)
{
	// Base no hace nada — subclases sobreescriben
}