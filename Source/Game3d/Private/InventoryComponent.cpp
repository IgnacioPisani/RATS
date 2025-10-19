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

FItemStruct UInventoryComponent::GetItemByIndex(int32 Index)
{
	if (Items.IsValidIndex(Index))
	{
		return Items[Index];
	}

	return FItemStruct();
}

int32 UInventoryComponent::GetItemQuantityByName(FName ItemName) const
{
	for (const FItemStruct& Item : Items)
	{
		if (Item.Name == ItemName)
		{
			return Item.Quantity;
		}
	}
	return 0; // Si no se encuentra, devolver 0
}

void UInventoryComponent::AddItem(FItemStruct ItemData)
{
	UE_LOG(LogTemp, Warning, TEXT("Agregando item: %s, Cantidad: %d"), *ItemData.Name.ToString(), ItemData.Quantity);

	bool bAdded = false;

	for (FItemStruct& Item : Items) // iteramos por referencia
	{
		if (ItemData.Name == Item.Name && ItemData.bStackable)
		{
			int32 EspacioDisponible = Item.MaxQuantity - Item.Quantity;

			if (EspacioDisponible > 0)
			{
				int32 CantidadAgregada = FMath::Min(ItemData.Quantity, EspacioDisponible);
				Item.Quantity += CantidadAgregada;

				UE_LOG(LogTemp, Warning, TEXT("Stackeado! Nueva cantidad: %d / %d"), Item.Quantity, Item.MaxQuantity);

				if (CantidadAgregada < ItemData.Quantity)
				{
					UE_LOG(LogTemp, Warning, TEXT("Stack lleno, sobró cantidad: %d"), ItemData.Quantity - CantidadAgregada);
				}

				OnItemsChanged.Broadcast();
				bAdded = true;
				break;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("El stack de %s está lleno (%d/%d)"), *Item.Name.ToString(), Item.Quantity, Item.MaxQuantity);
				bAdded = true; // Ya está lleno, no agregamos pero no es un error
				break;
			}
		}
	}

	if (!bAdded)
	{
		// Si no existe, agregamos el nuevo item limitado por su MaxQuantity
		ItemData.Quantity = FMath::Clamp(ItemData.Quantity, 1, ItemData.MaxQuantity);
		Items.Insert(ItemData, 0);
		UE_LOG(LogTemp, Warning, TEXT("Nuevo item agregado: %s, Cantidad: %d / %d"), *ItemData.Name.ToString(), ItemData.Quantity, ItemData.MaxQuantity);

		OnItemsChanged.Broadcast();
	}
}

bool UInventoryComponent::CraftItem(FName ItemNameToCraft)
{
	if (!CraftingDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("No hay DataTable de crafteo asignada."));
		return false;
	}

	static const FString ContextString(TEXT("Crafting Lookup"));
	const FItemStruct* RecipeRow = CraftingDataTable->FindRow<FItemStruct>(ItemNameToCraft, ContextString);

	if (!RecipeRow)
	{
		UE_LOG(LogTemp, Warning, TEXT("No se encontró la receta '%s' en la DataTable."), *ItemNameToCraft.ToString());
		return false;
	}

	const FCraftingRecipe* Recipe = CraftingRecipes.FindByPredicate(
		[&](const FCraftingRecipe& R) { return R.ResultItemName == ItemNameToCraft; });

	if (!Recipe)
	{
		UE_LOG(LogTemp, Warning, TEXT("No existe receta definida para %s"), *ItemNameToCraft.ToString());
		return false;
	}

	// 🔹 1. Verificar materiales
	for (const TPair<FName, int32>& RequiredPair : Recipe->RequiredItems)
	{
		int32 TotalFound = 0;
		for (const FItemStruct& Item : Items)
		{
			if (Item.Name == RequiredPair.Key)
				TotalFound += Item.Quantity;
		}

		if (TotalFound < RequiredPair.Value)
		{
			UE_LOG(LogTemp, Warning, TEXT("No hay suficientes %s. Necesitas %d y tienes %d"),
				*RequiredPair.Key.ToString(), RequiredPair.Value, TotalFound);
			return false;
		}
	}

	// 🔹 2. Verificar si hay espacio para el ítem resultante
	bool bCanAdd = false;

	for (const FItemStruct& Item : Items)
	{
		if (Item.Name == RecipeRow->Name)
		{
			if (Item.bStackable && Item.Quantity < Item.MaxQuantity)
			{
				int32 EspacioDisponible = Item.MaxQuantity - Item.Quantity;
				if (EspacioDisponible >= RecipeRow->Quantity)
				{
					bCanAdd = true;
					break;
				}
			}
			else
			{
				// No stackeable y ya existe → no se puede agregar otro
				bCanAdd = false;
			}
		}
	}

	// Si no existe en el inventario y se puede agregar nuevo
	if (!bCanAdd)
	{
		// Si no está en el inventario, verificamos si hay espacio para insertar
		bCanAdd = true; // Si tu inventario tiene límite de slots, acá podrías chequearlo
	}

	if (!bCanAdd)
	{
		UE_LOG(LogTemp, Warning, TEXT("No hay espacio para el ítem crafteado '%s'."), *ItemNameToCraft.ToString());
		return false;
	}

	// 🔹 3. Quitar materiales usados (solo si hay espacio)
	for (const TPair<FName, int32>& RequiredPair : Recipe->RequiredItems)
	{
		int32 Remaining = RequiredPair.Value;
		for (FItemStruct& Item : Items)
		{
			if (Item.Name == RequiredPair.Key)
			{
				int32 Deduct = FMath::Min(Item.Quantity, Remaining);
				Item.Quantity -= Deduct;
				Remaining -= Deduct;
				if (Remaining <= 0) break;
			}
		}
	}

	// 🔹 4. Eliminar ítems con cantidad 0
	Items.RemoveAll([&](const FItemStruct& I)
	{
		return Recipe->RequiredItems.Contains(I.Name) && I.Quantity <= 0;
	});

	// 🔹 5. Agregar el ítem crafteado al inventario (respetando MaxQuantity)
	FItemStruct CraftedItem = *RecipeRow;
	CraftedItem.Quantity = FMath::Clamp(CraftedItem.Quantity, 1, CraftedItem.MaxQuantity);
	AddItem(CraftedItem);

	UE_LOG(LogTemp, Log, TEXT("¡Se creó correctamente el ítem: %s!"), *ItemNameToCraft.ToString());
	return true;
}


const FItemStruct* UInventoryComponent::FindRecipe(FName ItemNameToCraft) const
{
	if (!CraftingDataTable) return nullptr;

	static const FString ContextString(TEXT("Crafting Recipe Lookup"));
	return CraftingDataTable->FindRow<FItemStruct>(ItemNameToCraft, ContextString);
}

bool UInventoryComponent::ConsumeItem(FName ItemName, int32 Quantity)
{
	
	if (Quantity <= 0) return false;

	for (FItemStruct& Item : Items)
	{
		if (Item.Name == ItemName)
		{
			if (Item.Quantity >= Quantity)
			{
				Item.Quantity -= Quantity;

				// Si se queda sin cantidad, lo consideramos vacío
				if (Item.Quantity <= 0)
				{
					Item.Quantity = 0;
					Item.Name = FName("Empty"); // opcional: marcar como espacio vacío
				}
				return true;
			}
		}
	}

	return false; // No se encontró el item
}
