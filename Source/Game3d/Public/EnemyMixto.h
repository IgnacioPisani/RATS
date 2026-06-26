// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyMelee.h"
#include "GameFramework/Character.h"
#include "EnemyMixto.generated.h"

class AEnemyDistance;

UCLASS()
class GAME3D_API AEnemyMixto : public AEnemyMelee
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyMixto();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ShootInDirection(FVector Direction);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Fire")
	TSubclassOf<AActor> BulletClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Summon")
	TSubclassOf<AEnemyDistance> DistanceEnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Summon")
	float SummonSpawnRadius = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Enrage")
	float Tier1SpeedMultiplier = 1.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Enrage")
	float Tier2SpeedMultiplier = 1.6f;

	float BaseWalkSpeed = 0.f; // cacheada en BeginPlay

	void ApplyEnrageSpeed(float SpeedMultiplier);
	
	UPROPERTY()
	TArray<AEnemyDistance*> SummonedAllies;

	// Cuántos umbrales de tercio ya gatillaron invocación (0,1,2)
	int32 ThirdsTriggered = 0;
	
	void TrySummonDistanceAllies(int32 Count);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Summon")
	int32 MaxSummonedAllies = 5;

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	AActor* TargetActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float FireRate = 2.f;  // segundos entre disparos

	float FireCooldown = 0.f;  // interno, no expuesto

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float DetectionRadius = 1500.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class UBoxComponent* MeleeHitBox;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float MeleeHitBoxDuration = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float MeleeDamage = 10.f;

	FTimerHandle MeleeHitBoxTimerHandle;
	
	virtual void HandleHit_Implementation() override;

	
	void HandleLifeChanged(float NewHealth, float NewMaxHealth);
	UFUNCTION()
	void ActivateMeleeHitBox();

	UFUNCTION()
	void DeactivateMeleeHitBox();

	UFUNCTION()
	void OnMeleeHitBoxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
};
