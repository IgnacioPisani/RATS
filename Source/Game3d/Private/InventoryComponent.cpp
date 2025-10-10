// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	Items.SetNum(10);
	// ...
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInventoryComponent::RemoveItem(int Index)
{
	if (Items.IsValidIndex(Index))
	{
		FItemStruct EmptyItem;
		Items[Index] = EmptyItem;
		OnItemsChanged.Broadcast();
		UE_LOG(LogTemp, Log, TEXT("Item en índice %d reemplazado por vacío."), Index);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("RemoveItem: Índice %d fuera de rango."), Index);
	}
}

bool  UInventoryComponent::GetItemByIndex(int32 Index, FItemStruct& OutItem)
{
	if (Items.IsValidIndex(Index))
	{
		OutItem = Items[Index];
		return true; 
	}

	return false; 
}

void UInventoryComponent::AddItem(FItemStruct ItemData)
{
	UE_LOG(LogTemp, Warning, TEXT("Agregando item: %s, Cantidad: %d"), *ItemData.Name.ToString(), ItemData.Quantity);

	bool bAdded = false;

	for (FItemStruct& Item : Items) // & importante, iteramos por referencia
	{
		UE_LOG(LogTemp, Warning, TEXT("Revisando item en inventario: %s, Cantidad: %d"), *Item.Name.ToString(), Item.Quantity);

		if (ItemData.Name.ToString().Equals(Item.Name.ToString())
			&& ItemData.bStackable
			&& (Item.Quantity + ItemData.Quantity) <= 64)
		{
			Item.Quantity += ItemData.Quantity;
			UE_LOG(LogTemp, Warning, TEXT("Stackeado! Nueva cantidad: %d"), Item.Quantity);
			OnItemsChanged.Broadcast();
			bAdded = true;
			break;
		}
	}

	if (!bAdded)
	{
		Items.Insert(ItemData, 0); // Inserta el item en la posición 0

		// Log del item agregado
		FItemStruct& NewItem = Items[0];
		OnItemsChanged.Broadcast();
	}
}
TSubclassOf<class AActor> UInventoryComponent::GetItemActorClass(int32 Index)
{
	if (Items.IsValidIndex(Index))
	{
		return Items[Index].ActorClass; 
	}
	return nullptr;
}
