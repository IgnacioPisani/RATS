#include "GemPedestal.h"
#include "Net/UnrealNetwork.h"
#include "GemCarrierInterface.h"

AGemPedestal::AGemPedestal()
{
	bReplicates = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	RootComponent = BaseMesh;

	SupportPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SupportPoint"));
	SupportPoint->SetupAttachment(RootComponent);

	RequiredColor = EGemColor::Red;
	bIsActivated = false;
}

void AGemPedestal::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGemPedestal, bIsActivated);
}

void AGemPedestal::OnRep_bIsActivated()
{
	if (bIsActivated)
	{
		OnPedestalActivated();
	}
}

void AGemPedestal::Interact_Implementation(AActor* Interactor)
{
	// Asume ejecución server-side, igual que el resto de tu lógica de Interact/RPCs
	if (bIsActivated || !Interactor || !Interactor->Implements<UGemCarrierInterface>())
	{
		return;
	}

	AGemItem* HeldGem = IGemCarrierInterface::Execute_GetHeldGem(Interactor);
	if (!HeldGem)
	{
		return;
	}

	if (HeldGem->GemColor != RequiredColor)
	{
		OnWrongGemPlaced();
		return;
	}

	HeldGem->PlaceOnSupport(SupportPoint);
	IGemCarrierInterface::Execute_SetHeldGem(Interactor, nullptr);

	bIsActivated = true;

	// OnRep no corre en el servidor, así que lo disparamos manualmente acá también
	// (importante en Listen Server: para que el host vea el efecto sin esperar el rep)
	OnPedestalActivated();
}
