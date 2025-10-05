// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"

#include "HealthBar.h"
#include "HealthComponent.h"
#include "Components/WidgetComponent.h"
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

