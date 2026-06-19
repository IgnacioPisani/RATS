#include "Tumbleweed.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"

ATumbleweed::ATumbleweed()
{
    PrimaryActorTick.bCanEverTick = true;

    // Mesh
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh;

    // Fisicas
    Mesh->SetSimulatePhysics(true);
    Mesh->SetEnableGravity(true);

    // Colision (solo mundo)
    Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
    Mesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

    // Ajustes de estabilidad
    Mesh->SetLinearDamping(0.05f);
    Mesh->SetAngularDamping(0.2f);

    // Masa fija: asi las fuerzas de viento/torque dan un resultado
    // predecible sin importar el mesh que se use.
    Mesh->SetMassOverrideInKg(NAME_None, 5.f, true);
}

void ATumbleweed::BeginPlay()
{
    Super::BeginPlay();

    if (!Mesh)
    {
        return;
    }

    PlaySpawnFX();

    // Direccion base del viento
    WindDirection = FVector(1.f, 0.3f, 0.f).GetSafeNormal();

    // Impulso inicial (para arrancar)
    Mesh->AddImpulse(WindDirection * InitialImpulse, NAME_None, true);

    // Autodestruir
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            DestroyTimer,
            this,
            &ATumbleweed::HandleDestroy,
            LifeSpan,
            false
        );
    }
}

void ATumbleweed::HandleDestroy()
{
    // FX antes de desaparecer
    PlaySpawnFX();

    // Usamos un weak pointer para no crashear si el actor ya fue
    // destruido por otro lado antes de que dispare este timer.
    TWeakObjectPtr<ATumbleweed> WeakThis(this);

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            DestroyDelayTimer,
            [WeakThis]()
            {
                if (WeakThis.IsValid())
                {
                    WeakThis->Destroy();
                }
            },
            0.2f,
            false
        );
    }
}

void ATumbleweed::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!Mesh)
    {
        return;
    }

    // Viento constante
    Mesh->AddForce(WindDirection * WindStrength);

    // Torque para que ruede
    Mesh->AddTorqueInDegrees(
        FVector(0.f, 0.f, TorqueStrength),
        NAME_None,
        true
    );
}