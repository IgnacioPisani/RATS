// EnemyProjectile.cpp
#include "EnemyProjectile.h"

#include "EnemyBase.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

// ─────────────────────────────────────────────────────────────────
//  Constructor
// ─────────────────────────────────────────────────────────────────
AEnemyProjectile::AEnemyProjectile()
{
    PrimaryActorTick.bCanEverTick = false; // No necesitamos Tick

    // ── Colisión (root) ───────────────────────────────────────────
    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    CollisionSphere->SetSphereRadius(CollisionRadius);
    CollisionSphere->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    CollisionSphere->SetNotifyRigidBodyCollision(true); // Genera Hit Events
    RootComponent = CollisionSphere;

    // ── Movimiento ────────────────────────────────────────────────
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed          = InitialSpeed;
    ProjectileMovement->MaxSpeed              = MaxSpeed;
    ProjectileMovement->ProjectileGravityScale = GravityScale;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce         = false;

    // ── Niagara Trail ─────────────────────────────────────────────
    NiagaraTrail = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraTrail"));
    NiagaraTrail->SetupAttachment(RootComponent);
    NiagaraTrail->SetAutoActivate(true); // Se activa solo al spawnear

    // Vida máxima del proyectil
    InitialLifeSpan = LifeSpan;
}

// ─────────────────────────────────────────────────────────────────
//  BeginPlay
// ─────────────────────────────────────────────────────────────────
void AEnemyProjectile::BeginPlay()
{
    Super::BeginPlay();

    // Forzar velocidad desde las propiedades editables
    ProjectileMovement->InitialSpeed = InitialSpeed;
    ProjectileMovement->MaxSpeed     = MaxSpeed;
    ProjectileMovement->ProjectileGravityScale = GravityScale;
    ProjectileMovement->Velocity     = GetActorForwardVector() * InitialSpeed;

    CollisionSphere->OnComponentHit.AddDynamic(this, &AEnemyProjectile::OnHit);

    if (TrailEffect && NiagaraTrail)
    {
        NiagaraTrail->SetAsset(TrailEffect);
        NiagaraTrail->Activate();
    }
    
}

void AEnemyProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// ─────────────────────────────────────────────────────────────────
//  Impacto
// ─────────────────────────────────────────────────────────────────
void AEnemyProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                              const FHitResult& Hit)
{
    // Ignorar si choca consigo mismo o con el que lo disparó
    AActor* MyOwner = GetOwner();
    if (!OtherActor || OtherActor == this || OtherActor == MyOwner) return;
    if (OtherActor->IsA(AEnemyBase::StaticClass())) return;

    // Aplicar daño
    AController* OwnerInstigator = MyOwner ? MyOwner->GetInstigatorController() : nullptr;

    UGameplayStatics::ApplyDamage(
        OtherActor,
        Damage,
        OwnerInstigator,
        this,
        UDamageType::StaticClass()
    );

    // Knockback al actor golpeado
    if (UPrimitiveComponent* PrimComp = OtherActor->FindComponentByClass<UPrimitiveComponent>())
    {
        if (PrimComp->IsSimulatingPhysics())
        {
            FVector KnockbackDir = GetVelocity().GetSafeNormal();
            PrimComp->AddImpulse(KnockbackDir * KnockbackForce, NAME_None, true);
        }
    }

    // Efecto de impacto Niagara
    SpawnImpactEffect(Hit.ImpactPoint, Hit.ImpactNormal.Rotation());

    // Destruir el proyectil
    Destroy();
}

void AEnemyProjectile::SpawnImpactEffect(const FVector& Location, const FRotator& Rotation)
{
    if (!ImpactEffect || !GetWorld()) return;

    UNiagaraFunctionLibrary::SpawnSystemAtLocation(
        GetWorld(),
        ImpactEffect,
        Location,
        Rotation,
        FVector(1.f),   // Scale
        true,           // Auto destroy
        true            // Auto activate
    );
}