// EnemyDistance.cpp
#include "EnemyDistance.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "DrawDebugHelpers.h"

// ─────────────────────────────────────────────────────────────────
//  Constructor
// ─────────────────────────────────────────────────────────────────
AEnemyDistance::AEnemyDistance()
{
    PrimaryActorTick.bCanEverTick = true;

    // ── Zona de vuelo (debug / overlap opcional) ──────────────────
    FlyZoneSphere = CreateDefaultSubobject<USphereComponent>(TEXT("FlyZoneSphere"));
    FlyZoneSphere->SetupAttachment(RootComponent);
    FlyZoneSphere->SetSphereRadius(150.f);
    FlyZoneSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // ── Percepción ────────────────────────────────────────────────

    UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius                 = DetectionRadius;
    SightConfig->LoseSightRadius             = DetectionRadius + 400.f;
    SightConfig->PeripheralVisionAngleDegrees = 90.f;
    SightConfig->SetMaxAge(5.f);
    SightConfig->DetectionByAffiliation.bDetectEnemies   = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals  = false;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = false;

    UAISenseConfig_Hearing* HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
    HearingConfig->HearingRange = 1000.f;
    HearingConfig->SetMaxAge(3.f);
    HearingConfig->DetectionByAffiliation.bDetectEnemies   = true;
    HearingConfig->DetectionByAffiliation.bDetectNeutrals  = false;
    HearingConfig->DetectionByAffiliation.bDetectFriendlies = false;

}

// ─────────────────────────────────────────────────────────────────
//  BeginPlay
// ─────────────────────────────────────────────────────────────────
void AEnemyDistance::BeginPlay()
{
    Super::BeginPlay();

    // Configurar movimiento volador
    UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    if (MoveComp)
    {
        MoveComp->SetMovementMode(MOVE_Flying);
        MoveComp->MaxFlySpeed          = FlySpeed;
        MoveComp->BrakingDecelerationFlying = 800.f;
        MoveComp->GravityScale         = 0.f;   // Sin gravedad
        MoveComp->bOrientRotationToMovement = true;
        MoveComp->RotationRate          = FRotator(0.f, 360.f, 0.f);
    }

    // Sin rotación automática de la cámara/controlador
    bUseControllerRotationYaw = false;

    // Vincular percepción
}

// ─────────────────────────────────────────────────────────────────
//  Tick
// ─────────────────────────────────────────────────────────────────
void AEnemyDistance::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    // Detección simple por distancia
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

    // Efecto hover siempre activo
    ApplyHoverEffect(DeltaTime);

    // Máquina de estados
    UpdateState();

    switch (CurrentState)
    {
    case EEnemyDistanceState::Idle:
        HandleIdleState(DeltaTime);
        break;
    case EEnemyDistanceState::Chasing:
        HandleChasingState(DeltaTime);
        break;
    case EEnemyDistanceState::Attacking:
        HandleAttackingState(DeltaTime);
        break;
    case EEnemyDistanceState::Retreating:
        HandleRetreatingState(DeltaTime);
        break;
    default:
        break;
    }
}

// ─────────────────────────────────────────────────────────────────
//  Máquina de estados: transiciones
// ─────────────────────────────────────────────────────────────────
void AEnemyDistance::UpdateState()
{
    if (CurrentState == EEnemyDistanceState::Dead)
        return;

    if (!TargetActor || !IsValid(TargetActor))
    {
        CurrentState = EEnemyDistanceState::Idle;
        return;
    }

    const float Dist = GetDistanceToTarget();

    if (Dist <= MinKeepDistance)
    {
        CurrentState = EEnemyDistanceState::Retreating;
    }
    else if (Dist <= AttackRange && CanSeeTarget())
    {
        CurrentState = EEnemyDistanceState::Attacking;
    }
    else
    {
        CurrentState = EEnemyDistanceState::Chasing;
    }
}

// ─────────────────────────────────────────────────────────────────
//  Estados
// ─────────────────────────────────────────────────────────────────
void AEnemyDistance::HandleIdleState(float DeltaTime)
{
    // Patrulla simple: mirar alrededor o quedarse quieto
    // Opcionalmente podrías mover a un punto de patrulla aquí
    MaintainFlyHeight(DeltaTime);
}

void AEnemyDistance::HandleChasingState(float DeltaTime)
{
    if (!TargetActor) return;

    FVector TargetLoc = TargetActor->GetActorLocation();
    // Acercarse pero quedándose a AttackRange
    FVector ToTarget   = (TargetLoc - GetActorLocation()).GetSafeNormal();
    FVector DesiredLoc = TargetLoc - ToTarget * (AttackRange * 0.9f);
    DesiredLoc.Z       = TargetLoc.Z + PreferredFlyHeight;

    MoveTowardsTarget(DesiredLoc, DeltaTime);
    FaceTarget(DeltaTime);
}

void AEnemyDistance::HandleAttackingState(float DeltaTime)
{
    if (!TargetActor) return;

    // Mantenerse en posición de disparo
    FVector TargetLoc  = TargetActor->GetActorLocation();
    FVector ToTarget   = (TargetLoc - GetActorLocation()).GetSafeNormal();
    FVector HoverPoint = TargetLoc - ToTarget * (AttackRange * 0.75f);
    HoverPoint.Z       = TargetLoc.Z + PreferredFlyHeight;

    MoveTowardsTarget(HoverPoint, DeltaTime);
    FaceTarget(DeltaTime);

    // Intentar disparar
    TryFire();
}

void AEnemyDistance::HandleRetreatingState(float DeltaTime)
{
    if (!TargetActor) return;
    RetreatFromTarget(DeltaTime);
    FaceTarget(DeltaTime);
    TryFire(); // Dispara mientras huye
}

// ─────────────────────────────────────────────────────────────────
//  Movimiento
// ─────────────────────────────────────────────────────────────────
void AEnemyDistance::MoveTowardsTarget(const FVector& TargetLocation, float DeltaTime)
{
    FVector Direction = (TargetLocation - GetActorLocation());
    float   Distance  = Direction.Size();

    if (Distance < 50.f) return; // Ya está cerca

    Direction.Normalize();
    AddMovementInput(Direction, 1.f);
    MaintainFlyHeight(DeltaTime);
}

void AEnemyDistance::RetreatFromTarget(float DeltaTime)
{
    if (!TargetActor) return;

    FVector AwayDir = (GetActorLocation() - TargetActor->GetActorLocation()).GetSafeNormal();
    AwayDir.Z       = 0.f; // Retrocede horizontalmente
    AddMovementInput(AwayDir, 1.f);
    MaintainFlyHeight(DeltaTime);
}

void AEnemyDistance::MaintainFlyHeight(float DeltaTime)
{
    // Raycast hacia abajo para medir la distancia al suelo
    FVector Start = GetActorLocation();
    FVector End   = Start - FVector(0.f, 0.f, 2000.f);

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    float CurrentHeight = PreferredFlyHeight;

    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic, Params))
    {
        CurrentHeight = Start.Z - Hit.ImpactPoint.Z;
    }

    float HeightError = PreferredFlyHeight - CurrentHeight;
    FVector Correction(0.f, 0.f, HeightError * HeightCorrectionStrength * DeltaTime);
    AddMovementInput(Correction.GetSafeNormal(), FMath::Abs(HeightError) * 0.01f);
}

void AEnemyDistance::ApplyHoverEffect(float DeltaTime)
{
    HoverTimer += DeltaTime;
    float HoverOffset = FMath::Sin(HoverTimer * HoverFrequency * PI * 2.f) * HoverAmplitude;

    FVector CurrentLoc = GetActorLocation();
    CurrentLoc.Z += HoverOffset * DeltaTime; // mueve el actor completo (capsula + mesh)
    SetActorLocation(CurrentLoc);
}

void AEnemyDistance::FaceTarget(float DeltaTime)
{
    if (!TargetActor) return;

    FVector ToTarget   = (TargetActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
    FRotator LookRot   = ToTarget.Rotation();
    FRotator CurrentRot = GetActorRotation();
    FRotator NewRot     = FMath::RInterpTo(CurrentRot, LookRot, DeltaTime, 5.f);
    NewRot.Pitch        = 0.f; // Opcional: no inclinar el cuerpo
    SetActorRotation(NewRot);
}

// ─────────────────────────────────────────────────────────────────
//  Disparo
// ─────────────────────────────────────────────────────────────────
void AEnemyDistance::TryFire()
{
    if (FireCooldown > 0.f) return;
    if (!ProjectileClass) return;
    if (!TargetActor || !CanSeeTarget()) return;

    // ── Reproducir montage de ataque ─────────────────────────────
    if (AttackMontage)
    {
        UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
        if (AnimInstance)
        {
            AnimInstance->Montage_Play(AttackMontage);
            UE_LOG(LogTemp, Warning, TEXT("Montage reproducido"));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("AnimInstance es null"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("AttackMontage no asignado en BP"));
    }
    
    // ... resto del código




    USkeletalMeshComponent* MeshComp = GetMesh();
    if (!MeshComp) return;

    FVector Origin = MeshComp->DoesSocketExist(MuzzleSocketName)
                        ? MeshComp->GetSocketLocation(MuzzleSocketName)
                        : GetActorLocation();

    FVector  TargetLoc = TargetActor->GetActorLocation();
    FRotator ShootRot  = (TargetLoc - Origin).Rotation();

    SpawnProjectile(Origin, ShootRot);
    FireCooldown = FireRate;
}

void AEnemyDistance::SpawnProjectile(const FVector& Origin, const FRotator& Rotation)
{
    if (!GetWorld() || !ProjectileClass) return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner   = this;
    SpawnParams.Instigator = GetInstigator();
    SpawnParams.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    AActor* Projectile = GetWorld()->SpawnActor<AActor>(
        ProjectileClass, Origin, Rotation, SpawnParams);

    if (!Projectile) return;

    // Si tu proyectil tiene un componente de daño configurable, ponlo aquí.
    // Ejemplo genérico con UProjectileMovementComponent:
    //   UProjectileMovementComponent* ProjMove =
    //       Projectile->FindComponentByClass<UProjectileMovementComponent>();
    //   if (ProjMove) ProjMove->InitialSpeed = 1200.f;

    UE_LOG(LogTemp, Log, TEXT("AEnemyDistance: Proyectil disparado hacia %s"),
           *TargetActor->GetName());
}

// ─────────────────────────────────────────────────────────────────
//  Utilidades
// ─────────────────────────────────────────────────────────────────
bool AEnemyDistance::CanSeeTarget() const
{
    if (!TargetActor || !GetWorld()) return false;

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    Params.AddIgnoredActor(TargetActor);

    bool bBlocked = GetWorld()->LineTraceSingleByChannel(
        Hit,
        GetActorLocation(),
        TargetActor->GetActorLocation(),
        ECC_Visibility,
        Params);

    return !bBlocked;
}

float AEnemyDistance::GetDistanceToTarget() const
{
    if (!TargetActor) return TNumericLimits<float>::Max();
    return FVector::Dist(GetActorLocation(), TargetActor->GetActorLocation());
}

// ─────────────────────────────────────────────────────────────────
//  Percepción IA
// ─────────────────────────────────────────────────────────────────
void AEnemyDistance::OnTargetPerceived(AActor* Actor, FAIStimulus Stimulus)
{
    if (!Actor) return;

    // Solo nos interesa el jugador (o cualquier Pawn enemigo)
    if (!Actor->IsA(APawn::StaticClass())) return;

    if (Stimulus.WasSuccessfullySensed())
    {
        TargetActor = Actor;
        UE_LOG(LogTemp, Log, TEXT("AEnemyDistance: Objetivo detectado → %s"), *Actor->GetName());
    }
    else
    {
        // Perdimos la percepción, pero podríamos seguir el último punto conocido
        if (TargetActor == Actor)
        {
            UE_LOG(LogTemp, Log, TEXT("AEnemyDistance: Objetivo perdido."));
            // Opcional: TargetActor = nullptr; para volver a Idle
        }
    }
}

// ─────────────────────────────────────────────────────────────────
//  Overrides de combate
// ─────────────────────────────────────────────────────────────────
void AEnemyDistance::HandleHit_Implementation()
{
    TargetActor = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}

void AEnemyDistance::HandleDeath()
{
    CurrentState = EEnemyDistanceState::Dead;
    TargetActor  = nullptr;

    // Deshabilitar movimiento
    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->DisableMovement();
    }

    // Aquí podrías lanzar animación de muerte, ragdoll, etc.
    Super::HandleDeath(); // Llama a Destroy() desde EnemyBase
}