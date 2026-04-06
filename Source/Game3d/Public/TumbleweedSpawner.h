// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TumbleweedSpawner.generated.h"

class ATumbleweed;

UCLASS()
class GAME3D_API ATumbleweedSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATumbleweedSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void SpawnTumbleweed();
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ATumbleweed> TumbleweedClass;

	UPROPERTY(EditAnywhere)
	float SpawnRadius = 3000.f;

	UPROPERTY(EditAnywhere)
	float SpawnInterval = 6.f;

	FTimerHandle SpawnTimer;

};
