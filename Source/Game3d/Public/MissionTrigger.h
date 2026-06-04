// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MissionTrigger.generated.h"

UCLASS()
class GAME3D_API AMissionTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMissionTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// BP_MissionTrigger.h
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
	UPROPERTY(EditAnywhere, Category = "Mission")
	float Xp = 0.f;
private:
	void ExecuteMissionUpdate();

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* Trigger;

	// Equivalente al Do Once
	bool bHasTriggered = false;

	UPROPERTY(EditAnywhere, Category = "Mission")
	FString NewMissionToSet;

	UPROPERTY(EditAnywhere, Category = "Mission")
	FString CurrentMission;

	// Equivalente a Tiempo
	UPROPERTY(EditAnywhere, Category = "Mission")
	float Tiempo = 0.f;
	
	FTimerHandle DelayTimerHandle;
};
