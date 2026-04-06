// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tumbleweed.generated.h"

UCLASS()
class GAME3D_API ATumbleweed : public AActor
{
	GENERATED_BODY()
	
public:
	void Tick(float DeltaTime);
	ATumbleweed();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* Movement;

	UPROPERTY(EditAnywhere)
	float InitialSpeed = 300.f;

	float GroundOffset = 20.f;
	float TraceDistance = 200.f;
	FTimerHandle DestroyTimer;
	void HandleDestroy();
	FVector WindDirection;

	UFUNCTION(BlueprintImplementableEvent, Category="FX")
	void PlaySpawnFX();
};