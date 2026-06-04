// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "CombatDamageable.h"
#include "HandleHit.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"

UCLASS()
class GAME3D_API AEnemyBase : public ACharacterBase, public ICombatDamageable
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyBase();

	virtual void HandleLifeChanged(float Health, float MaxHealth) override;
	
	virtual void HandleDeath() override;
	
	virtual void HandleHit_Implementation() override;

	virtual void TakeDamageEffects() override;

	UPROPERTY(EditAnywhere, Category="Damage")
	FName PelvisBoneName;
	
	// Widget Component que contendrá el HealthBar
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="UI")
	class UWidgetComponent* HealthBarWidgetComponent;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// EnemyBase.h

	UPROPERTY()
	AActor* LastDamageCauser;

	UPROPERTY(EditAnywhere, Category = "XP")
	float XpReward = 100.f;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void ApplyDamage(float Damage, AActor* DamageCauser, const FVector& DamageLocation, const FVector& DamageImpulse) override;
	virtual void ApplyHealing(float Healing, AActor* Healer) override;

	UFUNCTION(BlueprintImplementableEvent, Category="Combat")
	void ReceivedDamage(float Damage, const FVector& ImpactPoint, const FVector& DamageDirection);

};
