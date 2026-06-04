// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyMixto.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemyMixto::AEnemyMixto()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemyMixto::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyMixto::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Detectar jugador
	AActor* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (Player && FVector::Dist(GetActorLocation(), Player->GetActorLocation()) <= DetectionRadius)
	{
		TargetActor = Player;
	}
	else
	{
		TargetActor = nullptr;
	}

	// Cooldown de disparo
	if (FireCooldown > 0.f)
		FireCooldown -= DeltaTime;
}

// Called to bind functionality to input
void AEnemyMixto::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemyMixto::ShootInDirection(FVector Direction)
{
	if (FireCooldown > 0.f) return;
	if (!BulletClass) return;
	if (!TargetActor || !IsValid(TargetActor)) return;

	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp) return;

	FVector Origin = MeshComp->DoesSocketExist(TEXT("gun"))
		? MeshComp->GetSocketLocation(TEXT("gun"))
		: GetActorLocation();

	FVector  TargetLoc = TargetActor->GetActorLocation();
	FRotator ShootRot  = (TargetLoc - Origin).Rotation();

	// ── SpawnProjectile inline — igual que EnemyDistance ────────
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner      = this;
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AActor* Bullet = GetWorld()->SpawnActor<AActor>(
		BulletClass, Origin, ShootRot, SpawnParams);

	if (!Bullet) return;

	FireCooldown = FireRate;
	UE_LOG(LogTemp, Log, TEXT("AEnemyMixto: Proyectil disparado hacia %s"),
		*TargetActor->GetName());
}