#include "TumbleweedSpawner.h"
#include "Tumbleweed.h"
#include "TimerManager.h"
#include "Engine/World.h"

ATumbleweedSpawner::ATumbleweedSpawner()
{
}

void ATumbleweedSpawner::BeginPlay()
{
	Super::BeginPlay();

	// ⏱️ Arranca el timer que spawnea continuamente
	GetWorld()->GetTimerManager().SetTimer(
		SpawnTimer,
		this,
		&ATumbleweedSpawner::SpawnTumbleweed,
		SpawnInterval,
		true
	);
	
}

void ATumbleweedSpawner::SpawnTumbleweed()
{
	if (!TumbleweedClass) return;

	// 📍 Posición base del spawner
	FVector Origin = GetActorLocation();

	// 🎲 Offset random en X/Y
	FVector Offset = FVector(
		FMath::FRandRange(-SpawnRadius, SpawnRadius),
		FMath::FRandRange(-SpawnRadius, SpawnRadius),
		0.f
	);

	FVector SpawnLocation = Origin + Offset;

	// 🌵 Spawn del arbusto
	GetWorld()->SpawnActor<ATumbleweed>(
		TumbleweedClass,
		SpawnLocation,
		FRotator::ZeroRotator
	);
}

void ATumbleweedSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AddActorLocalRotation(FRotator(0.f, 500.f * DeltaTime, 0.f));

}
