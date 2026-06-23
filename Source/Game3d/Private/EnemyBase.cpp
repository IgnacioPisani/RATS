// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"

#include "BossHUD.h"
#include "Engine/DamageEvents.h"
#include "HealthBar.h"
#include "HealthComponent.h"
#include "XpComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

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

	// ── UI de jefe ──────────────────────────────
	if (bIsBoss && BossHUDWidgetClass)
	{
		// Ocultar la health bar flotante normal, el jefe usa su propio HUD
		if (HealthBarWidgetComponent)
		{
			HealthBarWidgetComponent->SetVisibility(false);
		}

		BossHUDWidgetInstance = CreateWidget<UBossHUD>(GetWorld(), BossHUDWidgetClass);
		if (BossHUDWidgetInstance)
		{
			BossHUDWidgetInstance->AddToViewport();
		}
	}
}

void AEnemyBase::HandleLifeChanged(float Health, float MaxHealth)
{
	if (HealthBarWidget)
	{
		HealthBarWidget->UpdateBar(Health, MaxHealth);
	}

	if (bIsBoss && BossHUDWidgetInstance)
	{
		BossHUDWidgetInstance->UpdateBar(Health, MaxHealth);
	}
}

void AEnemyBase::HandleDeath()
{
	bIsDead = true;
	OnRep_IsDead();

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->Montage_Play(DeathMontage);
	}

	if (LastDamageCauser)
	{
		if (UXpComponent* XpComp = LastDamageCauser->FindComponentByClass<UXpComponent>())
		{
			XpComp->IncreaseXp(XpReward);
		}
	}

	if (bIsBoss && BossHUDWidgetInstance)
	{
		BossHUDWidgetInstance->RemoveFromParent();
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
	StartHitFlash();

}

void AEnemyBase::StartHitFlash()
{
	// Aplicar overlay rojo
	if (HitFlashMaterial)
	{
		GetMesh()->SetOverlayMaterial(HitFlashMaterial);
		UE_LOG(LogTemp, Warning, TEXT("Overlay: %s"),
	GetMesh()->GetOverlayMaterial()
	? *GetMesh()->GetOverlayMaterial()->GetName()
	: TEXT("NULL"));
	}

	CurrentFlashCount = 0;

	GetWorldTimerManager().SetTimer(
		FlashTimerHandle,
		this,
		&AEnemyBase::UpdateHitFlash,
		HitFlashDuration,
		false  // no loop, solo espera y saca el overlay
	);
}

void AEnemyBase::UpdateHitFlash()
{
	CurrentFlashCount++;

	if (CurrentFlashCount >= HitFlashCount)
	{
		// Sacar overlay
		GetMesh()->SetOverlayMaterial(nullptr);
		GetWorldTimerManager().ClearTimer(FlashTimerHandle);
	}
	else
	{
		// Alternar overlay
		UMaterialInterface* Current = GetMesh()->GetOverlayMaterial();
		GetMesh()->SetOverlayMaterial(Current ? nullptr : HitFlashMaterial);

		GetWorldTimerManager().SetTimer(
			FlashTimerHandle,
			this,
			&AEnemyBase::UpdateHitFlash,
			HitFlashDuration,
			false
		);
	}
}


void AEnemyBase::OnDeathTimerExpired()
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

void AEnemyBase::ApplyDamage(float Damage, AActor* DamageCauser,
	const FVector& DamageLocation, const FVector& DamageImpulse)
{
	LastDamageCauser = DamageCauser;

	FDamageEvent DamageEvent;
	const float ActualDamage = TakeDamage(Damage, DamageEvent, nullptr, DamageCauser);

	if (ActualDamage > 0.0f)
	{
		GetCharacterMovement()->AddImpulse(DamageImpulse, true);

		if (GetMesh()->IsSimulatingPhysics())
		{
			GetMesh()->AddImpulseAtLocation(
				DamageImpulse * GetMesh()->GetMass(),
				DamageLocation);
		}

		ReceivedDamage(ActualDamage, DamageLocation, DamageImpulse.GetSafeNormal());
	}
}

void AEnemyBase::ApplyHealing(float Healing, AActor* Healer)
{
}

void AEnemyBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AEnemyBase, bIsDead);
}

void AEnemyBase::OnRep_IsDead()
{
	UE_LOG(LogTemp, Warning, TEXT("[EnemyBase] OnRep_IsDead ejecutado en: %s"), *GetName());

	SetActorEnableCollision(false);
	UE_LOG(LogTemp, Warning, TEXT("[EnemyBase] Colisión deshabilitada"));

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		UE_LOG(LogTemp, Warning, TEXT("[EnemyBase] MovementMode antes: %d"), (int32)MoveComp->MovementMode);
        
		MoveComp->StopMovementImmediately();
		UE_LOG(LogTemp, Warning, TEXT("[EnemyBase] StopMovementImmediately ejecutado"));
        
		MoveComp->DisableMovement();
		UE_LOG(LogTemp, Warning, TEXT("[EnemyBase] DisableMovement ejecutado - MovementMode ahora: %d"), (int32)MoveComp->MovementMode);
        
		MoveComp->SetComponentTickEnabled(false);
		UE_LOG(LogTemp, Warning, TEXT("[EnemyBase] Tick del MovementComponent deshabilitado"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[EnemyBase] ERROR: No se encontró CharacterMovementComponent en %s"), *GetName());
	}
	GetWorldTimerManager().SetTimer(
	DeathTimerHandle,
	this,
	&AEnemyBase::OnDeathTimerExpired,
	DeathDelay,
	false
);
}
