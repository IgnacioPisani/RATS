#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h" // TODO: reemplazar por el include real de tu interfaz interactuable
#include "GemItem.generated.h"

UENUM(BlueprintType)
enum class EGemColor : uint8
{
	Red,
	Blue
};

UCLASS()
class AGemItem : public AActor, public IInteractable // TODO: ajustar nombre de la interfaz si difiere
{
	GENERATED_BODY()

public:
	AGemItem();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gem")
	UStaticMeshComponent* GemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_GemColor, Category = "Gem")
	EGemColor GemColor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Gem")
	bool bIsHeld;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Gem")
	bool bIsPlaced;

	// TODO: ajustar la firma para que coincida exactamente con tu interfaz interactuable
	virtual void Interact_Implementation(AActor* Interactor) override;
	
	void PickUp(AActor* NewHolder, USceneComponent* HoldPoint);
	void PlaceOnSupport(USceneComponent* SupportPoint);
	
	void Drop(const FVector& DropLocation);
	
	void Throw(const FVector& LaunchVelocity, const FVector& SpawnLocation);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_GemColor();
	
	UPROPERTY(EditDefaultsOnly, Category = "Gem")
	UMaterialInterface* RedMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Gem")
	UMaterialInterface* BlueMaterial;
};
