// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"

UCLASS()
class GAME3D_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyBase();

	UFUNCTION()
	void HandleLifeChanged(float Health, float MaxHealth);

	UFUNCTION()
	void HandleDeath();
	
	virtual float TakeDamage(float DamageAmount,
						 struct FDamageEvent const& DamageEvent,
						 class AController* EventInstigator,
						 AActor* DamageCauser) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UHealthComponent* HealthComponent;
	// Widget Component que contendrá el HealthBar
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="UI")
	class UWidgetComponent* HealthBarWidgetComponent;

	// Puntero al UserWidget instanciado dentro del componente
	UPROPERTY()
	class UHealthBar* HealthBarWidget;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
