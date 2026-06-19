// Chest.h
#pragma once

#include "CoreMinimal.h"
#include "Mechanism.h"
#include "Components/PointLightComponent.h"
#include "NiagaraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Chest.generated.h"

UCLASS()
class GAME3D_API AChest : public AMechanism
{
	GENERATED_BODY()

public:
	AChest();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// ── Componentes adicionales del cofre ──────────────────────────────────

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USkeletalMeshComponent* MechanismMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UPointLightComponent* PointLight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UNiagaraComponent* NiagaraEffect;

	// ── Datos ──────────────────────────────────────────────────────────────

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Chest")
	TArray<FItemStruct> Contents;

	// Item que se spawnea al abrir (se limpia después del spawn)
	UPROPERTY(ReplicatedUsing=OnRep_SpawnItemData, EditAnywhere, BlueprintReadWrite, Category="Chest")
	FItemStruct SpawnItemData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Audio")
	USoundBase* ChestSound;

	// Clase del actor item a spawnear (asignar en BP: BP_Item, hijo de AItem)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Chest")
	TSubclassOf<AActor> ItemActorClass;

	// Offset de spawn relativo al cofre (editable en BP, default = lo visto en el BP original)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Chest")
	FVector ItemSpawnOffset = FVector(0.f, 0.f, 200.f);

	// ── Interface ──────────────────────────────────────────────────────────

	virtual FItemStruct GetItem_Implementation() override;
	virtual FItemStruct GetItemSpawn_Implementation() override;

protected:
	virtual void BeginPlay() override;

	virtual void OnActivated_Implementation(AActor* Interactor) override;

	// ── Replicación ────────────────────────────────────────────────────────

	UFUNCTION()
	void OnRep_SpawnItemData();

	// Multicast: efectos visuales de apertura (luz, niagara, sonido, animación)
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OpenChestEffects();

	// Multicast: ocultar luz y niagara tras el loot
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_EndLoot();

	// Spawnea el item actor en el servidor
	void Server_SpawnItemActor();

	// Timer handle para el delay de 2s antes de EndLoot
	FTimerHandle TimerHandle_EndLoot;

	// ── Blueprint ──────────────────────────────────────────────────────────

	UFUNCTION(BlueprintImplementableEvent, Category="Chest")
	void OpenChestUI(const TArray<FItemStruct>& ChestContents);
};