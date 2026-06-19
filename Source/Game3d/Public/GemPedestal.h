#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h" // TODO: reemplazar por el include real de tu interfaz interactuable
#include "GemItem.h"
#include "GemPedestal.generated.h"

UCLASS()
class AGemPedestal : public AActor, public IInteractable // TODO: ajustar nombre de la interfaz si difiere
{
	GENERATED_BODY()

public:
	AGemPedestal();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pedestal")
	UStaticMeshComponent* BaseMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pedestal")
	USceneComponent* SupportPoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pedestal")
	EGemColor RequiredColor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_bIsActivated, Category = "Pedestal")
	bool bIsActivated;

	UFUNCTION(BlueprintCallable, Category = "Pedestal")
	void DeactivatePedestal();

	// TODO: ajustar la firma para que coincida exactamente con tu interfaz interactuable
	virtual void Interact_Implementation(AActor* Interactor) override;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_bIsActivated();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Pedestal")
	void OnPedestalActivated();

	UFUNCTION(BlueprintImplementableEvent, Category = "Pedestal")
	void OnPedestalDeactivated();

	// Opcional: feedback visual/sonoro cuando ponen el color que no es
	UFUNCTION(BlueprintImplementableEvent, Category = "Pedestal")
	void OnWrongGemPlaced();
};
