// Fill out your copyright notice in the Description page of Project Settings.


#include "MissionGameState.h"

#include "Net/UnrealNetwork.h"

AMissionGameState::AMissionGameState()
{
	bReplicates = true;
}

void AMissionGameState::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMissionGameState, CurrentMission);
}

void AMissionGameState::UpdateMission_Implementation(const FString& NewMission)
{
	if (!HasAuthority()) return;

	CurrentMission = NewMission;

	OnRep_CurrentMission();
}

void AMissionGameState::OnRep_CurrentMission()
{
	OnMissionUpdated.Broadcast(CurrentMission);
}