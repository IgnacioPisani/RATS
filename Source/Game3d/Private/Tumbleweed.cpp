#include "Tumbleweed.h"
#include "Components/StaticMeshComponent.h"
ATumbleweed::ATumbleweed()
{
	PrimaryActorTick.bCanEverTick = true;

	// 🧱 Mesh
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	// 🔥 FÍSICAS
	Mesh->SetSimulatePhysics(true);
	Mesh->SetEnableGravity(true);

	// 🧠 COLISIÓN (solo mundo)
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

	// 🎯 AJUSTES DE ESTABILIDAD (CLAVE)
	Mesh->SetLinearDamping(0.05f);
	Mesh->SetAngularDamping(0.2f);
}

void ATumbleweed::BeginPlay()
{
	Super::BeginPlay();

	if (!Mesh) return;
    PlaySpawnFX();
	// 🌬️ Dirección base del viento
	WindDirection = FVector(1.f, 0.3f, 0.f).GetSafeNormal();

	// 🚀 IMPULSO INICIAL (para arrancar)
	float InitialImpulse = 800.f;
	Mesh->AddImpulse(WindDirection * InitialImpulse, NAME_None, true);

	// ⏳ Autodestruir
	GetWorld()->GetTimerManager().SetTimer(
		DestroyTimer,
		this,
		&ATumbleweed::HandleDestroy,
		12.f,
		false
	);}

void ATumbleweed::HandleDestroy()
{
	// 🔥 llamar FX en BP
	PlaySpawnFX();

	// ⏳ esperar un toque para que se vea
	FTimerHandle TempTimer;

	GetWorld()->GetTimerManager().SetTimer(
		TempTimer,
		[this]()
		{
			Destroy();
		},
		0.2f, // 👈 ajustable (0.1 - 0.3 ideal)
		false
	);
}

void ATumbleweed::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Mesh) return;

	// 🌪️ VIENTO CONSTANTE (FUERTE)
	float WindStrength = 90000.f;

	Mesh->AddForce(WindDirection * WindStrength);

	// 🌀 Torque para que ruede
	float TorqueStrength = 100000.f;

	Mesh->AddTorqueInDegrees(
		FVector(0.f, 0.f, TorqueStrength),
		NAME_None,
		true
	);
}
