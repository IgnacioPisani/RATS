#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "Engine/StaticMesh.h"
#include "ItemStruct.generated.h"

USTRUCT(BlueprintType)
struct  FItemStruct: public FTableRowBase
{
	GENERATED_BODY()

	// Nombre visible del item
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
	FName Name = NAME_None;

	// Indica si el item puede apilarse
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
	bool bStackable = false;

	// Cantidad actual
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
	int32 Quantity = 0;

	// Imagen de vista previa
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
	UTexture2D* Thumbnail = nullptr;

	// Malla 3D asociada
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
	UStaticMesh* Mesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
	int32 MaxQuantity = 99;

};
