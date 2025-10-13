// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"
#include "Engine/DamageEvents.h"
#include "HealthBar.h"
#include "HealthComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	HealthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	HealthBarWidgetComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	if (HealthComponent)
	{
		HealthComponent->OnLifeChanged.AddDynamic(this, &AEnemyBase::HandleLifeChanged);
		HealthComponent->OnDeath.AddDynamic(this, &AEnemyBase::HandleDeath);
	}
	
	if (UUserWidget* Widget = HealthBarWidgetComponent->GetUserWidgetObject())
	{
		HealthBarWidget = Cast<UHealthBar>(Widget);

		if (HealthBarWidget)
		{
			HealthBarWidget->SetBarColor(FLinearColor::Red);
		}
	}
}


void AEnemyBase::HandleLifeChanged(float Health, float MaxHealth)
{
	if (HealthBarWidget)
	{
		HealthBarWidget->UpdateBar(Health, MaxHealth);
	}
}

void AEnemyBase::HandleHit_Implementation()
{

}

void AEnemyBase::TakeDamageEffects()
{
	Super::TakeDamageEffects();
	GetMesh()->SetPhysicsBlendWeight(0.5f);
	GetMesh()->SetBodySimulatePhysics(PelvisBoneName, false);
}

void AEnemyBase::HandleDeath()
{
	Destroy();
}


// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyBase::ApplyDamage(float Damage, AActor* DamageCauser, const FVector& DamageLocation,
	const FVector& DamageImpulse)
{
	// pass the damage event to the actor
	FDamageEvent DamageEvent;
	const float ActualDamage = TakeDamage(Damage, DamageEvent, nullptr, DamageCauser);

	// only process knockback and effects if we received nonzero damage
	if (ActualDamage > 0.0f)
	{
		// apply the knockback impulse
		GetCharacterMovement()->AddImpulse(DamageImpulse, true);

		// is the character ragdolling?
		if (GetMesh()->IsSimulatingPhysics())
		{
			// apply an impulse to the ragdoll
			GetMesh()->AddImpulseAtLocation(DamageImpulse * GetMesh()->GetMass(), DamageLocation);
		}
		ReceivedDamage(ActualDamage, DamageLocation, DamageImpulse.GetSafeNormal());

	}

}

void AEnemyBase::ApplyHealing(float Healing, AActor* Healer)
{
}


