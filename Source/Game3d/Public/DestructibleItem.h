// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatDamageable.h"
#include "GameFramework/Actor.h"
#include "DestructibleItem.generated.h"

UCLASS()
class GAME3D_API ADestructibleItem : public AActor, public ICombatDamageable
{
	GENERATED_BODY()
	
public:
	ADestructibleItem();

	void BreakItem(const FVector& HitDirection);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, Category="Break")
	TSubclassOf<class ABrokenItem> BrokenItemClass;

	UPROPERTY(EditAnywhere, Category="Break")
	float Health = 1.f;

public:
	

	// --- Interfaz ---
	virtual void ApplyDamage(
		float Damage,
		AActor* DamageCauser,
		const FVector& DamageLocation,
		const FVector& DamageImpulse
	) override;

	virtual void HandleDeath() override;

	virtual void ApplyHealing(
		float Healing,
		AActor* Healer
	) override;

private:

	void Break(const FVector& Impulse);
};