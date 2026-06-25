// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"

// -------------------------------------------------------
// Constructor
// -------------------------------------------------------
UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// IMPORTANT: this tells Unreal to replicate the component
	// and all of its Replicated properties to connected clients.
	SetIsReplicatedByDefault(true);

	Items.SetNum(10);
}

// -------------------------------------------------------
// Replication registration
// -------------------------------------------------------
void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
\
	// Replicate Items to every connected client unconditionally.
	// Swap COND_None for COND_OwnerOnly if you want the inventory
	// to be private (only the owning player receives updates).
	DOREPLIFETIME(UInventoryComponent, Items);
}

// -------------------------------------------------------
// OnRep – fires on every CLIENT when Items replicates down
// -------------------------------------------------------
void UInventoryComponent::OnRep_Items()
{
	// The array is already updated by the time this callback runs.
	// Just notify any UI or gameplay code that is listening.
	OnItemsChanged.Broadcast();
}

// -------------------------------------------------------
// Lifecycle
// -------------------------------------------------------
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

// ===============================================================
//  Public blueprint-callable wrappers
//  These are what Blueprints and gameplay code call.
//  If we already have authority (server / standalone) run the
//  logic directly.  Otherwise send the RPC to the server.
// ===============================================================

void UInventoryComponent::AddItem(FItemStruct ItemData)
{
	if (GetOwner()->HasAuthority())
		Internal_AddItem(ItemData);
	else
		Server_AddItem(ItemData);
}

void UInventoryComponent::RemoveItem(int32 Index)
{
	if (GetOwner()->HasAuthority())
		Internal_RemoveItem(Index);
	else
		Server_RemoveItem(Index);
}

bool UInventoryComponent::CraftItem(FName ItemNameToCraft)
{
	if (GetOwner()->HasAuthority())
		return Internal_CraftItem(ItemNameToCraft);

	Server_CraftItem(ItemNameToCraft);
	// Return value is meaningless on the client here;
	// the server will broadcast OnItemsChanged if it succeeds.
	return false;
}

bool UInventoryComponent::ConsumeItem(FName ItemName, int32 Quantity)
{
	if (GetOwner()->HasAuthority())
		return Internal_ConsumeItem(ItemName, Quantity);

	Server_ConsumeItem(ItemName, Quantity);
	return false;
}

// ===============================================================
//  Server RPC implementations
//  Unreal generates Server_AddItem_Implementation / _Validate
//  from the UFUNCTION(Server, Reliable, WithValidation) macros.
// ===============================================================

// --- AddItem ---
bool UInventoryComponent::Server_AddItem_Validate(FItemStruct ItemData)
{
	// Basic sanity checks. Returning false kicks the client.
	return !ItemData.Name.IsNone() && ItemData.Quantity > 0;
}
void UInventoryComponent::Server_AddItem_Implementation(FItemStruct ItemData)
{
	Internal_AddItem(ItemData);
}

// --- RemoveItem ---
bool UInventoryComponent::Server_RemoveItem_Validate(int32 Index)
{
	return Items.IsValidIndex(Index);
}
void UInventoryComponent::Server_RemoveItem_Implementation(int32 Index)
{
	Internal_RemoveItem(Index);
}

// --- CraftItem ---
bool UInventoryComponent::Server_CraftItem_Validate(FName ItemNameToCraft)
{
	return !ItemNameToCraft.IsNone();
}
void UInventoryComponent::Server_CraftItem_Implementation(FName ItemNameToCraft)
{
	Internal_CraftItem(ItemNameToCraft);
}

// --- ConsumeItem ---
bool UInventoryComponent::Server_ConsumeItem_Validate(FName ItemName, int32 Quantity)
{
	return !ItemName.IsNone() && Quantity > 0;
}
void UInventoryComponent::Server_ConsumeItem_Implementation(FName ItemName, int32 Quantity)
{
	Internal_ConsumeItem(ItemName, Quantity);
}

// ===============================================================
//  Internal implementations – authority-only
//  Identical to your original logic, but now the array mutation
//  is guaranteed to happen only on the server, and Unreal will
//  automatically replicate the change to all clients, triggering
//  OnRep_Items on each one.
// ===============================================================

void UInventoryComponent::Internal_RemoveItem(int32 Index)
{
	if (Items.IsValidIndex(Index))
	{
		FItemStruct EmptyItem;
		Items[Index] = EmptyItem;
		// Broadcast on the server immediately (clients get it via OnRep).
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
		return Items[Index];
	return FItemStruct();
}

int32 UInventoryComponent::GetItemQuantityByName(FName ItemName) const
{
	for (const FItemStruct& Item : Items)
	{
		if (Item.Name == ItemName)
			return Item.Quantity;
	}
	return 0;
}

void UInventoryComponent::Internal_AddItem(FItemStruct ItemData)
{
	UE_LOG(LogTemp, Warning, TEXT("Agregando item: %s, Cantidad: %d"), *ItemData.Name.ToString(), ItemData.Quantity);

	bool bAdded = false;

	for (FItemStruct& Item : Items)
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
					UE_LOG(LogTemp, Warning, TEXT("Stack lleno, sobró cantidad: %d"), ItemData.Quantity - CantidadAgregada);

				OnItemsChanged.Broadcast();
				bAdded = true;
				break;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("El stack de %s está lleno (%d/%d)"), *Item.Name.ToString(), Item.Quantity, Item.MaxQuantity);
				bAdded = true;
				break;
			}
		}
	}

	if (!bAdded)
	{
		ItemData.Quantity = FMath::Clamp(ItemData.Quantity, 1, ItemData.MaxQuantity);
		Items.Insert(ItemData, 0);
		UE_LOG(LogTemp, Warning, TEXT("Nuevo item agregado: %s, Cantidad: %d / %d"), *ItemData.Name.ToString(), ItemData.Quantity, ItemData.MaxQuantity);
		OnItemsChanged.Broadcast();
	}
}

bool UInventoryComponent::Internal_CraftItem(FName ItemNameToCraft)
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

	// 1. Verify materials
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

	// 2. Check space for the result item
	bool bCanAdd = false;
	for (const FItemStruct& Item : Items)
	{
		if (Item.Name == RecipeRow->Name)
		{
			if (Item.bStackable && Item.Quantity < Item.MaxQuantity)
			{
				if ((Item.MaxQuantity - Item.Quantity) >= RecipeRow->Quantity)
				{
					bCanAdd = true;
					break;
				}
			}
			else
			{
				return false;
			}
		}
	}
	if (!bCanAdd) bCanAdd = true; // Slot will be inserted fresh

	if (!bCanAdd)
	{
		UE_LOG(LogTemp, Warning, TEXT("No hay espacio para el ítem crafteado '%s'."), *ItemNameToCraft.ToString());
		return false;
	}

	// 3. Deduct materials
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

	// 4. Remove zero-quantity slots that were ingredients
	Items.RemoveAll([&](const FItemStruct& I)
	{
		return Recipe->RequiredItems.Contains(I.Name) && I.Quantity <= 0;
	});

	// 5. Add the crafted item
	FItemStruct CraftedItem = *RecipeRow;
	CraftedItem.Quantity = FMath::Clamp(CraftedItem.Quantity, 1, CraftedItem.MaxQuantity);
	Internal_AddItem(CraftedItem);

	UE_LOG(LogTemp, Log, TEXT("¡Se creó correctamente el ítem: %s!"), *ItemNameToCraft.ToString());
	return true;
}

const FItemStruct* UInventoryComponent::FindRecipe(FName ItemNameToCraft) const
{
	if (!CraftingDataTable) return nullptr;
	static const FString ContextString(TEXT("Crafting Recipe Lookup"));
	return CraftingDataTable->FindRow<FItemStruct>(ItemNameToCraft, ContextString);
}

bool UInventoryComponent::Internal_ConsumeItem(FName ItemName, int32 Quantity)
{
	if (Quantity <= 0) return false;

	for (FItemStruct& Item : Items)
	{
		if (Item.Name == ItemName)
		{
			if (Item.Quantity >= Quantity)
			{
				Item.Quantity -= Quantity;
				if (Item.Quantity <= 0)
				{
					Item.Quantity = 0;
					Item.Name = FName("Empty");
				}
				OnItemsChanged.Broadcast();
				return true;
			}
		}
	}
	return false;
}