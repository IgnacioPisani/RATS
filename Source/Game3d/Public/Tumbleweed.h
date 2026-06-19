#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tumbleweed.generated.h"

class UStaticMeshComponent;

UCLASS()
class ATumbleweed : public AActor
{
	GENERATED_BODY()

public:
	ATumbleweed();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tumbleweed")
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tumbleweed")
	float WindStrength = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tumbleweed")
	float TorqueStrength = 10000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tumbleweed")
	float InitialImpulse = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tumbleweed")
	float LifeSpan = 4.f;

	// Implementar en el Blueprint. Se llama tanto al spawnear como al
	// empezar la destruccion (para el FX de "desaparicion").
	UFUNCTION(BlueprintImplementableEvent, Category = "Tumbleweed")
	void PlaySpawnFX();

private:
	void HandleDestroy();

	FVector WindDirection;

	FTimerHandle DestroyTimer;
	FTimerHandle DestroyDelayTimer;
};