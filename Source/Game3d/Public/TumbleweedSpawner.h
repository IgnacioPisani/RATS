#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TumbleweedSpawner.generated.h"

class ATumbleweed;

UCLASS()
class ATumbleweedSpawner : public AActor
{
	GENERATED_BODY()

public:
	ATumbleweedSpawner();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// Clase Blueprint del Tumbleweed a spawnear (asignar en el editor)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tumbleweed")
	TSubclassOf<ATumbleweed> TumbleweedClass;

	// Tiempo entre cada spawn, en segundos. OJO: si esto queda en 0 spawnea
	// un actor por frame y eso es lo que rompe el juego.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tumbleweed", meta = (ClampMin = "0.05"))
	float SpawnInterval = 4.0f;

	// Radio de dispersion del spawn alrededor del spawner
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tumbleweed")
	float SpawnRadius = 5000.f;

	// Maximo de tumbleweeds vivos al mismo tiempo (evita que se acumulen para siempre)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tumbleweed")
	int32 MaxTumbleweeds = 2;

private:
	void SpawnTumbleweed();

	UFUNCTION()
	void HandleTumbleweedDestroyed(AActor* DestroyedActor);

	FTimerHandle SpawnTimer;

	int32 CurrentTumbleweedCount = 0;
};