#include "EnemyDistance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemyDistance::AEnemyDistance()
{
	PrimaryActorTick.bCanEverTick = true;
	
}

void AEnemyDistance::BeginPlay()
{
	Super::BeginPlay();

	// ✈️ volar
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
}

void AEnemyDistance::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemyDistance::HandleHit_Implementation()
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
	}}
