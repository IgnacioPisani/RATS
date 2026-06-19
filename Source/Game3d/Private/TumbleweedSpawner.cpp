#include "TumbleweedSpawner.h"
#include "Tumbleweed.h"
#include "TimerManager.h"
#include "Engine/World.h"

ATumbleweedSpawner::ATumbleweedSpawner()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATumbleweedSpawner::BeginPlay()
{
	Super::BeginPlay();

	// Por si en el editor quedo en 0 o negativo, nunca dejamos que el timer
	// dispare cada frame.
	const float SafeInterval = FMath::Max(SpawnInterval, 0.05f);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			SpawnTimer,
			this,
			&ATumbleweedSpawner::SpawnTumbleweed,
			SafeInterval,
			true
		);
	}
}

void ATumbleweedSpawner::SpawnTumbleweed()
{
	if (!TumbleweedClass)
	{
		return;
	}

	if (CurrentTumbleweedCount >= MaxTumbleweeds)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// Posicion base del spawner
	const FVector Origin = GetActorLocation();

	// Offset random en X/Y
	const FVector Offset = FVector(
		FMath::FRandRange(-SpawnRadius, SpawnRadius),
		FMath::FRandRange(-SpawnRadius, SpawnRadius),
		0.f
	);

	const FVector SpawnLocation = Origin + Offset;

	ATumbleweed* NewTumbleweed = World->SpawnActor<ATumbleweed>(
		TumbleweedClass,
		SpawnLocation,
		FRotator::ZeroRotator
	);

	if (NewTumbleweed)
	{
		CurrentTumbleweedCount++;
		// Cuando el tumbleweed se destruya solo, bajamos el contador.
		NewTumbleweed->OnDestroyed.AddDynamic(this, &ATumbleweedSpawner::HandleTumbleweedDestroyed);
	}
}

void ATumbleweedSpawner::HandleTumbleweedDestroyed(AActor* DestroyedActor)
{
	CurrentTumbleweedCount = FMath::Max(0, CurrentTumbleweedCount - 1);
}

void ATumbleweedSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AddActorLocalRotation(FRotator(0.f, 500.f * DeltaTime, 0.f));
}