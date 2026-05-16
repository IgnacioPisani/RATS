// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UpdateMission.h"
#include "GameFramework/GameStateBase.h"
#include "MissionGameState.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnMissionUpdated,
	const FString&,
	NewMission
);
UCLASS()
class GAME3D_API AMissionGameState : public AGameStateBase, public IUpdateMission
{
	GENERATED_BODY()

protected:

	UPROPERTY(ReplicatedUsing=OnRep_CurrentMission)
	FString CurrentMission;

	UFUNCTION()
	void OnRep_CurrentMission();

	AMissionGameState();

public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void UpdateMission_Implementation(const FString& NewMission) override;

	FORCEINLINE FString GetCurrentMission() const { return CurrentMission; }

	UPROPERTY(BlueprintAssignable)
	FOnMissionUpdated OnMissionUpdated;
};
