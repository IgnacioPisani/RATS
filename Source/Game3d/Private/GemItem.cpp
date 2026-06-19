#include "GemItem.h"
#include "Net/UnrealNetwork.h"
#include "GemPedestal.h"
#include "GemCarrierInterface.h"

AGemItem::AGemItem()
{
	bReplicates = true;
	SetReplicateMovement(false); // el AttachToComponent ya replica la transform solo

	GemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GemMesh"));
	RootComponent = GemMesh;
	GemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	bIsHeld = false;
	bIsPlaced = false;
	GemColor = EGemColor::Red;
}

void AGemItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGemItem, GemColor);
	DOREPLIFETIME(AGemItem, bIsHeld);
	DOREPLIFETIME(AGemItem, bIsPlaced);
}

void AGemItem::OnRep_GemColor()
{
	UMaterialInterface* Mat = (GemColor == EGemColor::Red) ? RedMaterial : BlueMaterial;
	if (Mat)
	{
		GemMesh->SetMaterial(0, Mat);
	}
}

void AGemItem::Interact_Implementation(AActor* Interactor)
{
	if (bIsHeld || !Interactor)
	{
		return;
	}

	if (!Interactor->Implements<UGemCarrierInterface>())
	{
		return;
	}
	
	if (IGemCarrierInterface::Execute_GetHeldGem(Interactor) != nullptr)
	{
		return;
	}

	USceneComponent* HoldPoint = IGemCarrierInterface::Execute_GetGemHoldPoint(Interactor);
	if (!HoldPoint)
	{
		return;
	}

	PickUp(Interactor, HoldPoint);
	IGemCarrierInterface::Execute_SetHeldGem(Interactor, this);
}

void AGemItem::PickUp(AActor* NewHolder, USceneComponent* HoldPoint)
{
	AGemPedestal* Pedestal = Cast<AGemPedestal>(GetOwner());
	if (Pedestal)
	{
		Pedestal->DeactivatePedestal();
	}
	
	bIsHeld = true;
	bIsPlaced = false; // Reseteamos el estado al recogerla.
	SetOwner(NewHolder);

	GemMesh->SetSimulatePhysics(false);
	GemMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	AttachToComponent(HoldPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}

void AGemItem::PlaceOnSupport(USceneComponent* SupportPoint)
{
	bIsHeld = false;
	bIsPlaced = true;

	AttachToComponent(SupportPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}

void AGemItem::Drop(const FVector& DropLocation)
{
	bIsHeld  = false;
	bIsPlaced = false;
	SetOwner(nullptr);

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetActorLocation(DropLocation);

	GemMesh->SetSimulatePhysics(true);
	GemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SetReplicateMovement(true);
}

void AGemItem::Throw(const FVector& LaunchVelocity, const FVector& SpawnLocation)
{
	bIsHeld  = false;
	bIsPlaced = false;
	SetOwner(nullptr);

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetActorLocation(SpawnLocation);

	GemMesh->SetSimulatePhysics(true);
	GemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SetReplicateMovement(true);

	// Imprimimos la velocidad para que la física haga la parábola
	GemMesh->SetPhysicsLinearVelocity(LaunchVelocity);
}
