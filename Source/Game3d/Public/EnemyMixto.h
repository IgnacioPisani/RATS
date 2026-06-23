// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyMelee.h"
#include "GameFramework/Character.h"
#include "EnemyMixto.generated.h"

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

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	AActor* TargetActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float FireRate = 2.f;  // segundos entre disparos

	float FireCooldown = 0.f;  // interno, no expuesto

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float DetectionRadius = 1500.f;
	
};
