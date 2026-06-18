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

	// Este es el "soporte" que pediste: el punto exacto donde encaja el diamante.
	// Movelo en el editor (Viewport del actor/BP) a la zona exacta que quieras.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pedestal")
	USceneComponent* SupportPoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pedestal")
	EGemColor RequiredColor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_bIsActivated, Category = "Pedestal")
	bool bIsActivated;

	// TODO: ajustar la firma para que coincida exactamente con tu interfaz interactuable
	virtual void Interact_Implementation(AActor* Interactor) override;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_bIsActivated();

	// ESTE es el "mensaje" que pediste para activar la escalera (o lo que quieras).
	// Implementalo en el Blueprint hijo de esta clase: ahí movés la escalera con un Timeline,
	// reproducís un sonido, lo que sea — sin tocar C++.
	UFUNCTION(BlueprintImplementableEvent, Category = "Pedestal")
	void OnPedestalActivated();

	// Opcional: feedback visual/sonoro cuando ponen el color que no es
	UFUNCTION(BlueprintImplementableEvent, Category = "Pedestal")
	void OnWrongGemPlaced();
};
