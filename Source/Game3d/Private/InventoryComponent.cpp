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

	// Verificar materiales
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

	// Quitar materiales usados
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

	// Eliminar ítems con cantidad 0
	Items.RemoveAll([&](const FItemStruct& I)
	{
		// Solo eliminar si es un material consumido y Quantity <= 0
		return Recipe->RequiredItems.Contains(I.Name) && I.Quantity <= 0;
	});
	// Agregar el ítem crafteado al inventario
	AddItem(*RecipeRow);

	UE_LOG(LogTemp, Log, TEXT("¡Se creó correctamente el ítem: %s!"), *ItemNameToCraft.ToString());

	return true;
}


const FItemStruct* UInventoryComponent::FindRecipe(FName ItemNameToCraft) const
{
	if (!CraftingDataTable) return nullptr;

	static const FString ContextString(TEXT("Crafting Recipe Lookup"));
	return CraftingDataTable->FindRow<FItemStruct>(ItemNameToCraft, ContextString);
}
