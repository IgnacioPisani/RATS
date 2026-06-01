// EnemyProjectile.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class GAME3D_API AEnemyProjectile : public AActor
{
    GENERATED_BODY()

public:
    AEnemyProjectile();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // ─────────────────────────────────────────
    //  Componentes
    // ─────────────────────────────────────────
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<USphereComponent> CollisionSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

    /** Efecto Niagara que viaja con el proyectil */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UNiagaraComponent> NiagaraTrail;

    // ─────────────────────────────────────────
    //  Configuración
    // ─────────────────────────────────────────

    /** Daño que aplica al impactar */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    float Damage = 20.f;

    /** Velocidad inicial */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    float InitialSpeed = 150.f;

    /** Velocidad máxima */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    float MaxSpeed = 150.f;

    /** Gravedad (0 = recto, 1 = gravedad normal) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    float GravityScale = 0.f;

    /** Radio de la esfera de colisión */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    float CollisionRadius = 20.f;

    /** Tiempo de vida antes de destruirse solo */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    float LifeSpan = 5.f;

    /** Efecto Niagara al impactar */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
    TObjectPtr<UNiagaraSystem> ImpactEffect;

    /** Sistema Niagara que viaja con el proyectil (trail) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
    TObjectPtr<UNiagaraSystem> TrailEffect;

    // ─────────────────────────────────────────
    //  Impulso de knockback al impactar
    // ─────────────────────────────────────────
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    float KnockbackForce = 300.f;

private:
    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
               UPrimitiveComponent* OtherComp, FVector NormalImpulse,
               const FHitResult& Hit);

    void SpawnImpactEffect(const FVector& Location, const FRotator& Rotation);
};