#pragma once
#include "CoreMinimal.h"
#include "CraftingRecipe.generated.h"

USTRUCT(BlueprintType)
struct FCraftingRecipe
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ResultItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ResultQuantity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, int32> RequiredItems;
};
