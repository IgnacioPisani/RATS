#include "Item.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

class UInventoryComponent;
class UStaticMeshComponent;

// Sets default values
AItem::AItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Habilitar replicación del actor — sin esto, el item spawneado en
	// servidor jamás aparece en los clientes.
	bReplicates = true;
	SetReplicateMovement(true); // sincroniza Transform para items tangibles con física

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	RootComponent = StaticMesh;
}

void AItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AItem, ItemData);
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	ApplyMeshFromItemData();

	if (bIsTangible)
	{
		// Comportamiento original: item del mundo, recogible, con física.
		if (HasAuthority())
		{
			StaticMesh->SetSimulatePhysics(true);
		}
	}
	else
	{
		// Item flotante de drop (ej. al abrir un cofre): sin física,
		// sin colisión de pickup, solo rota y se autodestruye.
		StaticMesh->SetSimulatePhysics(false);
		StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// Autodestrucción por tiempo — solo el servidor decide cuándo destruir;
		// la destrucción se replica automáticamente a los clientes.
		if (HasAuthority() && LifeTime > 0.f)
		{
			GetWorldTimerManager().SetTimer(
				TimerHandle_Lifetime,
				[this]()
				{
					Destroy();
				},
				LifeTime,
				false
			);
		}
	}
}

void AItem::ApplyMeshFromItemData()
{
	if (ItemData.Mesh) // suponiendo que FItemStruct tiene un UStaticMesh* llamado Mesh
	{
		StaticMesh->SetStaticMesh(ItemData.Mesh);
	}
}

void AItem::OnRep_ItemData()
{
	// Se ejecuta en clientes cuando ItemData llega replicado.
	ApplyMeshFromItemData();
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsTangible)
	{
		// Rotación constante en Yaw — efecto "item flotando" tipo RPG.
		// Se ejecuta en todos (server y clientes) usando el mismo DeltaTime
		// local, así que se ve fluido sin necesidad de replicar rotación.
		AddActorLocalRotation(FRotator(0.f, RotationSpeed * DeltaTime, 0.f));
	}
}

FItemStruct AItem::GetItem_Implementation()
{
	return ItemData;
}

void AItem::Interact_Implementation(AActor* Interactor)
{
}