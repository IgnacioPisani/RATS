// Chest.cpp
#include "Chest.h"

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
	OpenChestUI(Contents);
}
