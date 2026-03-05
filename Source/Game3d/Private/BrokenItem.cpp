#include "BrokenItem.h"
#include "Components/StaticMeshComponent.h"

ABrokenItem::ABrokenItem()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABrokenItem::BeginPlay()
{
	Super::BeginPlay();

	// Busca TODOS los StaticMeshComponent del BP
	GetComponents<UStaticMeshComponent>(Pieces);

	SetLifeSpan(LifeTime);
}

void ABrokenItem::ApplyBreakImpulse(const FVector& HitDirection)
{
	for (UStaticMeshComponent* Piece : Pieces)
	{
		if (Piece && Piece->IsSimulatingPhysics())
		{
			FVector RandomDir =
				(HitDirection + FMath::VRand() * 0.4f).GetSafeNormal();

			Piece->AddImpulse(RandomDir * ImpulseStrength);
		}
	}
}