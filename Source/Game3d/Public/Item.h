// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "ItemStruct.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UCLASS()
class GAME3D_API AItem : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItem();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> StaticMesh;

	// Replicado — el cliente necesita este dato para mostrar el mesh correcto.
	UPROPERTY(ReplicatedUsing=OnRep_ItemData, EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn="true"), Category="Inventory")
	FItemStruct ItemData;

	// false = item "flotante" de drop (no recogible con Interact, sin colisión
	// de overlap para pickup, no simula física — solo rota y se autodestruye).
	// true  = item normal del mundo, recogible (comportamiento original).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn="true"), Category="Inventory")
	bool bIsTangible = true;

	// Velocidad de rotación en Yaw (grados/segundo) — solo aplica si !bIsTangible
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	float RotationSpeed = 90.f;

	// Tiempo de vida antes de autodestruirse (segundos) — solo aplica si !bIsTangible
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn="true"), Category="Inventory")
	float LifeTime = 2.f;

	UFUNCTION()
	void OnRep_ItemData();

	virtual FItemStruct GetItem_Implementation();

	virtual void Interact_Implementation(AActor* Interactor) override;

protected:
	// Aplica ItemData.Mesh al StaticMeshComponent — usado en BeginPlay y OnRep
	void ApplyMeshFromItemData();

	FTimerHandle TimerHandle_Lifetime;
};