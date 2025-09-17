// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
class UStaticMeshComponent;

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	RootComponent = StaticMesh;

}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	if (ItemData.Mesh) // suponiendo que FItemStruct tiene un UStaticMesh* llamado Mesh
	{
		StaticMesh->SetStaticMesh(ItemData.Mesh);
	}

	// Activamos la simulacion de fisica
	StaticMesh->SetSimulatePhysics(true);
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FItemStruct AItem::GetItem_Implementation()
{
	return ItemData;
}

