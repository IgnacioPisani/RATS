#include "GemItem.h"
#include "Net/UnrealNetwork.h"
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
	// Esto asume que tu sistema de Interact ya llega acá solo en el servidor
	// (igual que tu Server_Fire para el disparo). Si no es así, avisame.
	if (bIsHeld || bIsPlaced || !Interactor)
	{
		return;
	}

	if (!Interactor->Implements<UGemCarrierInterface>())
	{
		return;
	}

	// No deja agarrar un segundo diamante si ya tiene uno en la mano
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
	bIsHeld = true;
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
