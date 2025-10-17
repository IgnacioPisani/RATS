// CraftingRecipeData.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CraftingRecipe.h"
#include "CraftingRecipeData.generated.h"

UCLASS(BlueprintType)
class GAME3D_API UCraftingRecipeData : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Crafting")
	TArray<FCraftingRecipe> Recipes;
};
