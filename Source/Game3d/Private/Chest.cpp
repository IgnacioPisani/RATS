// Chest.cpp
#include "Chest.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraFunctionLibrary.h"
#include "TimerManager.h"
#include "Item.h"

AChest::AChest()
{
	ItemData.Name = FName("Chest");

	// ── Skeletal Mesh del cofre (MechanismMesh en el BP) ──────────────────
	MechanismMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MechanismMesh"));
	MechanismMesh->SetupAttachment(RootComponent);

	// ── Point Light ───────────────────────────────────────────────────────
	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	PointLight->SetupAttachment(RootComponent);

	// ── Niagara ───────────────────────────────────────────────────────────
	NiagaraEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraEffect"));
	NiagaraEffect->SetupAttachment(RootComponent);
}

void AChest::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AChest, SpawnItemData);
}

void AChest::BeginPlay()
{
	Super::BeginPlay();

	// ── BeginPlay BP: SET ItemData ─────────────────────────────────────────
	// (imagen 1: Event BeginPlay → SET ItemData Name="Chest", Stackable=false,
	//  Quantity=0, MaxQuantity=99)
	// Esto ya se inicializa en el constructor, pero si necesitás sobreescribir
	// en runtime desde BP, dejamos el BeginPlay vacío y usamos los defaults.
	ItemData.Name         = FName("Chest");
	ItemData.bStackable   = false;
	ItemData.Quantity     = 0;
	ItemData.MaxQuantity  = 99;
	// Thumbnail y Mesh se asignan desde el editor en el BP derivado
}

// ── Interface ──────────────────────────────────────────────────────────────

FItemStruct AChest::GetItem_Implementation()
{
	return Super::GetItem_Implementation();
}

FItemStruct AChest::GetItemSpawn_Implementation()
{
	return SpawnItemData;
}

// ── OnActivated (Event Interact en el BP) ──────────────────────────────────
//
// Flujo BP traducido:
//  1. SET SpawnItemData desde parámetros del interactor
//  2. Branch: bIsActivated → si false, continúa
//  3. Play Animation (SkeletalMesh)
//  4. Play Sound 2D (ChestSound)
//  5. SET bIsActivated = true
//  6. SET Visibility PointLight = false
//  7. SET Visibility Niagara    = false
//  8. SpawnActor BP_Item en posición del cofre + offset
//  9. SET SpawnItemData vacío en el actor spawneado
// 10. Delay 2s → EndLoot
//
void AChest::OnActivated_Implementation(AActor* Interactor)
{
	// Solo corre en servidor (Interact_Implementation ya chequea HasAuthority)

	// Efectos visuales/sonoros para todos los clientes
	Multicast_OpenChestEffects();

	// Spawnear el item actor (solo en servidor)
	Server_SpawnItemActor();

	// Delay 2 segundos → EndLoot
	GetWorldTimerManager().SetTimer(
		TimerHandle_EndLoot,
		[this]()
		{
			Multicast_EndLoot();
		},
		2.0f,
		false
	);

	// Abrir UI del cofre (solo en cliente local — llamada desde servidor
	// no tiene efecto en clientes; si necesitás que el cliente abra la UI
	// usá un ClientRPC o un RepNotify)
	OpenChestUI(Contents);
}

// ── Efectos Multicast ──────────────────────────────────────────────────────

void AChest::Multicast_OpenChestEffects_Implementation()
{
	// Sonido (imagen 2: Play Sound 2D con ChestSound)
	if (ChestSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ChestSound, GetActorLocation());
	}

	// Animación (imagen 2: Play Animation en SkeletalMesh, Anim=Cofre12_Anim)
	PlayActivationAnimation(); // BlueprintImplementableEvent → BP la implementa

	// Ocultar PointLight y Niagara (imagen 2: SET Visibility false)
	if (PointLight)
	{
		PointLight->SetVisibility(false);
	}
	if (NiagaraEffect)
	{
		NiagaraEffect->SetVisibility(false);
	}
}

// ── Spawn del item actor (servidor) ───────────────────────────────────────
//
// Imagen 3:
//  - SpawnActor BP_Item
//  - Location = GetActorTransform().Location + (100, 0, 0) y + (0, 200, 0)
//    (los offsets 100 y 200 visibles en los nodos Add Pin)
//  - Scale = Multiply Scale With Root Component Scale
//  - Luego SET SpawnItemData vacío en el actor spawneado
//
void AChest::Server_SpawnItemActor()
{
	if (!HasAuthority() || !ItemActorClass) return;

	FVector ForwardVector = GetActorForwardVector();
	FVector BaseLocation  = GetActorLocation();

	FVector SpawnLocation = BaseLocation
		+ ForwardVector * ItemSpawnOffset.X
		+ FVector(0.f, 0.f, ItemSpawnOffset.Z);
	FRotator SpawnRotation = GetActorRotation();
	FTransform SpawnTransform(SpawnRotation, SpawnLocation);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = GetInstigator();

	// Deferred spawn: el actor se construye pero NO corre BeginPlay todavía.
	// Esto nos permite setear ItemData ANTES de que AItem::BeginPlay lo lea
	// para configurar el StaticMesh (igual que ExposeOnSpawn en Blueprint).
	AActor* DeferredActor = GetWorld()->SpawnActorDeferred<AActor>(
		ItemActorClass,
		SpawnTransform,
		nullptr,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);

	if (AItem* SpawnedItem = Cast<AItem>(DeferredActor))
	{
		// Setear el ItemData antes de finalizar el spawn
		SpawnedItem->ItemData = SpawnItemData;

		// Este item es el drop visual del cofre: flota, rota, se autodestruye.
		// No es recogible con Interact ni tiene física simulada.
		SpawnedItem->bIsTangible = false;

		// Finalizar el spawn → ahora sí corre BeginPlay con ItemData ya seteado
		SpawnedItem->FinishSpawning(SpawnTransform);
	}
	else if (DeferredActor)
	{
		// Por si ItemActorClass no es un AItem (clase custom distinta)
		DeferredActor->FinishSpawning(SpawnTransform);
	}

	// Limpiar SpawnItemData en este cofre después del spawn
	SpawnItemData = FItemStruct();
}

// ── EndLoot (imagen 4) ─────────────────────────────────────────────────────
//
// Oculta PointLight y Niagara en todos los clientes
//
void AChest::Multicast_EndLoot_Implementation()
{
	if (PointLight)
	{
		PointLight->SetVisibility(false);
	}
	if (NiagaraEffect)
	{
		NiagaraEffect->SetVisibility(false);
	}
}

// ── RepNotify SpawnItemData ────────────────────────────────────────────────

void AChest::OnRep_SpawnItemData()
{
	// Clientes pueden reaccionar al cambio de SpawnItemData si es necesario
}