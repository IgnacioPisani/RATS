#include "DestructibleItem.h"
#include "BrokenItem.h"
#include "Components/StaticMeshComponent.h"

ADestructibleItem::ADestructibleItem()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	Mesh->SetCollisionProfileName(TEXT("BlockAll"));
}

void ADestructibleItem::BeginPlay()
{
	Super::BeginPlay();
}

void ADestructibleItem::BreakItem(const FVector& HitDirection)
{
	if (!BrokenItemClass) return;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ABrokenItem* Broken = GetWorld()->SpawnActor<ABrokenItem>(
		BrokenItemClass,
		GetActorTransform(),
		Params
	);

	if (Broken)
	{
		Broken->ApplyBreakImpulse(HitDirection);
	}

	Destroy();
}

void ADestructibleItem::ApplyDamage(
	float Damage,
	AActor* DamageCauser,
	const FVector& DamageLocation,
	const FVector& DamageImpulse)
{
	UE_LOG(LogTemp, Warning, TEXT("Destructible recibió daño"));

	Health -= Damage;
	UE_LOG(LogTemp, Warning, TEXT("Health actual: %f"), Health);
	if (Health <= 0.f)
	{
		FVector HitDirection = DamageImpulse;

		if (HitDirection.IsNearlyZero() && DamageCauser)
		{
			HitDirection = GetActorLocation() - DamageCauser->GetActorLocation();
			HitDirection.Normalize();
		}

		BreakItem(HitDirection);
	}
}

void ADestructibleItem::HandleDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("Destructible murió"));
}

void ADestructibleItem::ApplyHealing(float Healing, AActor* Healer)
{
	// No hace nada. Es un objeto rompible.
}