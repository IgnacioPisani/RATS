#include "XpComponent.h"
#include "Net/UnrealNetwork.h"

UXpComponent::UXpComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);

	Xp = 0.f;
	MaxXp = 100.f;
	Level = 1;
}

void UXpComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UXpComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

// ==========================
// 🔥 SERVER LOGIC
// ==========================

void UXpComponent::IncreaseXp(float XpPoints)
{
	if (!GetOwner()->HasAuthority()) return;

	Xp += XpPoints;

	// Clamp opcional
	Xp = FMath::Clamp(Xp, 0.f, MaxXp);

	// 🔥 EVENTO SERVER (útil si sos listen server)
	OnXpChanged.Broadcast(Xp, MaxXp);

	if (Xp >= MaxXp)
	{
		IncreaseLevel(1);
	}
}

void UXpComponent::IncreaseLevel(int AddedLevel)
{
	if (!GetOwner()->HasAuthority()) return;

	Level += AddedLevel;
	MaxXp += 150.f;
	Xp = 0.f;

	// 🔥 EVENTOS SERVER
	OnLevelChanged.Broadcast(Level);
	OnXpChanged.Broadcast(Xp, MaxXp);
}

// ==========================
// 🔥 CLIENT SYNC (CLAVE)
// ==========================

void UXpComponent::OnRep_Xp()
{
	OnXpChanged.Broadcast(Xp, MaxXp);
}

void UXpComponent::OnRep_Level()
{
	OnLevelChanged.Broadcast(Level);
}

void UXpComponent::OnRep_MaxXp()
{
	OnXpChanged.Broadcast(Xp, MaxXp);
}

// ==========================
// 🔥 REPLICATION
// ==========================

void UXpComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UXpComponent, Xp);
	DOREPLIFETIME(UXpComponent, MaxXp);
	DOREPLIFETIME(UXpComponent, Level);
}