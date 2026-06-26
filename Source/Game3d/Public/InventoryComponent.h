// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CraftingRecipe.h"
#include "ItemStruct.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemsChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent, BlueprintType) )
class GAME3D_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

	// -------------------------------------------------------
	// Replicated inventory array.
	// OnRep_Items is called automatically on every client
	// whenever the server modifies this array.
	// -------------------------------------------------------
	UPROPERTY(ReplicatedUsing = OnRep_Items, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<FItemStruct> Items;

	// Called on clients when Items replicates down from the server.
	UFUNCTION()
	void OnRep_Items();

	// Required for Unreal's replication system to know which properties to replicate.
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// -------------------------------------------------------
	// Crafting data
	// -------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	TArray<FCraftingRecipe> CraftingRecipes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crafting")
	UDataTable* CraftingDataTable;

	// Delegate – broadcast locally on server AND on clients (via OnRep_Items).
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnItemsChanged OnItemsChanged;

	// -------------------------------------------------------
	// Blueprint-callable helpers (read-only – safe to call anywhere)
	// -------------------------------------------------------
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	FItemStruct GetItemByIndex(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetItemQuantityByName(FName ItemName) const;

	const FItemStruct* FindRecipe(FName ItemNameToCraft) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	TArray<FItemStruct> GetItems() const { return Items; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	int32 GetInventorySize() const { return Items.Num(); }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Crafting")
	FCraftingRecipe GetRecipeByName(FName RecipeName) const;

	// -------------------------------------------------------
	// Mutating actions: call the Server_ RPC from a client.
	// The server validates authority and runs the real logic.
	// -------------------------------------------------------

	/** Client-facing call. Internally routes to the Server RPC. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddItem(FItemStruct ItemData);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RemoveItem(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Crafting")
	bool CraftItem(FName ItemNameToCraft);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool ConsumeItem(FName ItemName, int32 Quantity);

private:
	// -------------------------------------------------------
	// Server RPCs  (Reliable = guaranteed delivery)
	// -------------------------------------------------------
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddItem(FItemStruct ItemData);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RemoveItem(int32 Index);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_CraftItem(FName ItemNameToCraft);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ConsumeItem(FName ItemName, int32 Quantity);

	// -------------------------------------------------------
	// Internal implementations – only called on the server
	// (HasAuthority() == true).
	// -------------------------------------------------------
	void Internal_AddItem(FItemStruct ItemData);
	void Internal_RemoveItem(int32 Index);
	bool Internal_CraftItem(FName ItemNameToCraft);
	bool Internal_ConsumeItem(FName ItemName, int32 Quantity);
};