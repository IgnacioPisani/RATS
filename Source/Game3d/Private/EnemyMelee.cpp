// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyMelee.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemyMelee::AEnemyMelee()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemyMelee::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyMelee::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyMelee::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyMelee::HandleHit_Implementation()
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp) return;

	FVector SocketLocation = MeshComp->GetSocketLocation(AttackSocketName);
	FRotator SocketRotation = MeshComp->GetSocketRotation(AttackSocketName);
	FVector ForwardVector = SocketRotation.Vector();
	FVector End = SocketLocation + ForwardVector * AttackRange;

	// Definimos qué tipo de objeto puede golpear (en este caso, el Character del jugador)
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	// Ignorar al propio enemigo
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	FHitResult OutHit;

	bool bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
		GetWorld(),
		SocketLocation,
		End,
		AttackRadius,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration, // Cambiar a None en producción
		OutHit,
		true
	);

	if (bHit)
	{
		AActor* HitActor = OutHit.GetActor();
		if (HitActor && HitActor->IsA(ACharacter::StaticClass()))
		{
			// Aplica daño al jugador
			UGameplayStatics::ApplyDamage(
				HitActor,           // Actor que recibe daño
				AttackDamage,       // Cantidad de daño
				GetController(),    // Instigador (controlador del enemigo)
				this,               // Causante del daño
				nullptr             // Tipo de daño (puede ser nullptr o una clase de DamageType)
			);
		}
	}
}

