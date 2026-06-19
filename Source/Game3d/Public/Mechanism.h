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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn="true"), Category="Inventory")
	FItemStruct ItemData;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UStaticMeshComponent* StaticMesh;

	// Replicado — cuando cambia en server, OnRep lo propaga a clientes
	UPROPERTY(ReplicatedUsing=OnRep_IsActivated, EditAnywhere, BlueprintReadWrite, Category="Mechanism")
	bool bIsActivated = false;

	UFUNCTION()
	virtual void OnRep_IsActivated();

	virtual FItemStruct GetItem_Implementation() override;
	virtual void Interact_Implementation(AActor* Interactor) override;

	// Llamado en server, ejecutado en todos los clientes (animación, sonido, VFX)
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayActivationEffects();

	// Implementable en Blueprint por subclase (animación de apertura, etc.)
	UFUNCTION(BlueprintImplementableEvent, Category="Mechanism")
	void PlayActivationAnimation();

	UFUNCTION(BlueprintNativeEvent, Category="Mechanism")
	void OnActivated(AActor* Interactor);
	virtual void OnActivated_Implementation(AActor* Interactor);
};