// Chest.cpp
#include "Chest.h"

#include "Kismet/GameplayStatics.h"

AChest::AChest()
{
	ItemData.Name = FName("Chest");
	// Configuración específica del cofre si es necesario
}

FItemStruct AChest::GetItem_Implementation()
{
	return Super::GetItem_Implementation();
}

FItemStruct AChest::GetItemSpawn_Implementation()
{
	return SpawnItemData;
}

void AChest::OnActivated_Implementation(AActor* Interactor)
{
	if (ChestSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			ChestSound,
			GetActorLocation()
		);
	} 
	OpenChestUI(Contents);
}
