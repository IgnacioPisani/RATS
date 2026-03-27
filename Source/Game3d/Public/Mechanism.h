// Mechanism.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "ItemStruct.h"
#include "Mechanism.generated.h"

UCLASS(Abstract)
class GAME3D_API AMechanism : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AMechanism();

	UPROPERTY(EditAnywhere, BlueprintReadWrite,  meta=(ExposeOnSpawn="true"),Category="Inventory")
	FItemStruct ItemData;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mechanism")
	bool bIsActivated = false;

	// Devuelve struct vacío por defecto — subclases con ítems lo sobreescriben
	virtual FItemStruct GetItem_Implementation() override;

	// Subclases implementan su propia lógica
	virtual void Interact_Implementation(AActor* Interactor) override;

	// Eventos implementables en Blueprint por cada subclase
	UFUNCTION(BlueprintImplementableEvent, Category="Mechanism")
	void PlayActivationAnimation();

	UFUNCTION(BlueprintNativeEvent, Category="Mechanism")
	void OnActivated(AActor* Interactor);
};