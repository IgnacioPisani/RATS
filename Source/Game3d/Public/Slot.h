// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemStruct.h"
#include "Blueprint/UserWidget.h"
#include "Slot.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemRemoved, int32, Index);

UCLASS()
class GAME3D_API USlot : public UUserWidget
{
	GENERATED_BODY()

public:
	// Variables visibles en el diseñador
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	FItemStruct Item;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	int32 Index;

	// Dispatcher
	UPROPERTY(BlueprintAssignable, Category="Inventory")
	FOnItemRemoved OnItemRemoved;

protected:


private:

};
