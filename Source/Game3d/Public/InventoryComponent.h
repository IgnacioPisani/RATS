// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CraftingRecipe.h"
#include "ItemStruct.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemsChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAME3D_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	TArray<FItemStruct> Items;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Lista de recetas disponibles
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	TArray<FCraftingRecipe> CraftingRecipes;
	
	UPROPERTY(BlueprintAssignable, Category="Inventory")
	FOnItemsChanged OnItemsChanged;
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")	
	void RemoveItem(int Index);

	UFUNCTION(BlueprintCallable, Category = "Inventory")	
	FItemStruct GetItemByIndex(int Index);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetItemQuantityByName(FName ItemName) const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Crafting")
	UDataTable* CraftingDataTable;
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")	
	void AddItem(FItemStruct ItemData);

	UFUNCTION(BlueprintCallable, Category = "Crafting")
	bool CraftItem(FName ItemNameToCraft);
	const FItemStruct* FindRecipe(FName ItemNameToCraft) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool ConsumeItem(FName ItemName, int32 Quantity);
};
