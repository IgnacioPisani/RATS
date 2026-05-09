// Fill out your copyright notice in the Description page of Project Settings.


#include "MissionTrigger.h"

#include "UpdateMission.h"
#include "Components/BoxComponent.h"

// Sets default values
AMissionTrigger::AMissionTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));

	RootComponent = Trigger;

	Trigger->SetBoxExtent(FVector(100.f));
}

// Called when the game starts or when spawned
void AMissionTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMissionTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMissionTrigger::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White,
		TEXT("Overlap detectado"));

	if (!OtherActor)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red,
			TEXT("OtherActor null"));
		return;
	}

	if (!OtherActor->GetClass()->ImplementsInterface(UUpdateMission::StaticClass()))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red,
			TEXT("NO implementa interfaz"));
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green,
		TEXT("Implementa interfaz"));

	if (bHasTriggered)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow,
			TEXT("Ya triggeredo"));
		return;
	}

	bHasTriggered = true;

	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan,
		TEXT("Ejecutando interfaz"));

	IUpdateMission::Execute_UpdateMission(OtherActor, NewMissionToSet);

}