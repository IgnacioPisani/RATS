// EnemyDistance.h
#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemyDistance.generated.h"

class UProjectileComponent;
class UAIPerceptionComponent;
class USphereComponent;

UENUM(BlueprintType)
enum class EEnemyDistanceState : uint8
{
    Idle       UMETA(DisplayName = "Idle"),
    Chasing    UMETA(DisplayName = "Chasing"),
    Attacking  UMETA(DisplayName = "Attacking"),
    Retreating UMETA(DisplayName = "Retreating"),
    Dead       UMETA(DisplayName = "Dead")
};

UCLASS()
class GAME3D_API AEnemyDistance : public AEnemyBase
{
    GENERATED_BODY()

public:
    AEnemyDistance();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
    virtual void HandleHit_Implementation() override;
    virtual void HandleDeath() override;

    // ─────────────────────────────────────────
    //  Componentes
    // ─────────────────────────────────────────
protected:
    /** Collider que define la "zona de vuelo" alrededor del enemigo */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    TObjectPtr<USphereComponent> FlyZoneSphere;

    // ─────────────────────────────────────────
    //  Parámetros de vuelo
    // ─────────────────────────────────────────
public:
    /** Altura preferida al volar sobre el suelo */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
    float PreferredFlyHeight = 300.f;

    /** Qué tan fuerte se ajusta la altura cada frame */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
    float HeightCorrectionStrength = 5.f;

    /** Oscilación vertical para efecto "hover" */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
    float HoverAmplitude = 20.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
    float HoverFrequency = 1.5f;

    /** Velocidad máxima en vuelo */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
    float FlySpeed = 600.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    TObjectPtr<UAnimMontage> AttackMontage;
    // ─────────────────────────────────────────
    //  Parámetros de combate
    // ─────────────────────────────────────────
public:
    /** Proyectil a disparar */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    TSubclassOf<AActor> ProjectileClass;

    /** Socket en el mesh desde donde sale el proyectil */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    FName MuzzleSocketName = TEXT("Muzzle");

    /** Daño por proyectil */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float ProjectileDamage = 20.f;

    /** Distancia mínima; si el jugador se acerca más, retrocede */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float MinKeepDistance = 500.f;

    /** Cadencia de disparo (segundos entre disparos) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float FireRate = 1.5f;

    /** Rango de detección inicial */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float DetectionRadius = 2000.f;

    // ─────────────────────────────────────────
    //  Estado interno
    // ─────────────────────────────────────────
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    EEnemyDistanceState CurrentState = EEnemyDistanceState::Idle;

    UPROPERTY()
    TObjectPtr<AActor> TargetActor;

    float FireCooldown = 0.f;
    float HoverTimer   = 0.f;

    // ─────────────────────────────────────────
    //  Funciones privadas
    // ─────────────────────────────────────────
private:
    void UpdateState();
    void HandleIdleState(float DeltaTime);
    void HandleChasingState(float DeltaTime);
    void HandleAttackingState(float DeltaTime);
    void HandleRetreatingState(float DeltaTime);

    void MoveTowardsTarget(const FVector& TargetLocation, float DeltaTime);
    void RetreatFromTarget(float DeltaTime);
    void MaintainFlyHeight(float DeltaTime);
    void ApplyHoverEffect(float DeltaTime);
    void FaceTarget(float DeltaTime);

    void TryFire();
    void SpawnProjectile(const FVector& Origin, const FRotator& Rotation);

    bool CanSeeTarget() const;
    float GetDistanceToTarget() const;
    void OnTargetPerceived(AActor* Actor, FAIStimulus Stimulus);
};