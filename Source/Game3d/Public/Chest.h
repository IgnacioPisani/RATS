// Chest.h
#pragma once

#include "CoreMinimal.h"
#include "Mechanism.h"
#include "Chest.generated.h"

UCLASS()
class GAME3D_API AChest : public AMechanism
{
	GENERATED_BODY()

public:
	AChest();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Chest")
	TArray<FItemStruct> Contents;

	virtual FItemStruct GetItem_Implementation() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Chest")
	FItemStruct SpawnItemData;
	
	FItemStruct GetItemSpawn_Implementation() override;
protected:

	// Sobreescribe OnActivated para abrir la UI del cofre
	virtual void OnActivated_Implementation(AActor* Interactor) override;

	UFUNCTION(BlueprintImplementableEvent, Category="Chest")
	void OpenChestUI(const TArray<FItemStruct>& ChestContents);
};