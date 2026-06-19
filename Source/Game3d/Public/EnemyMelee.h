// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "GameFramework/Character.h"
#include "EnemyMelee.generated.h"

UCLASS()
class GAME3D_API AEnemyMelee : public AEnemyBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyMelee();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void HandleHit_Implementation() override;

	UPROPERTY(ReplicatedUsing=OnRep_IsAttacking, BlueprintReadWrite, Category="Combat")
	bool bIsAttacking = false;

	UFUNCTION()
	void OnRep_IsAttacking();
};
