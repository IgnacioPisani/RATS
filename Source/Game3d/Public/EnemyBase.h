// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "CombatDamageable.h"
#include "HandleHit.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"

class UBossHUD;

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
	void StartHitFlash();
	void UpdateHitFlash();

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float DeathDelay = 0.f;  // ajustable en BP
	
	UPROPERTY(EditAnywhere, Category = "XP")
	float XpReward = 100.f;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void ApplyDamage(float Damage, AActor* DamageCauser, const FVector& DamageLocation, const FVector& DamageImpulse) override;
	virtual void ApplyHealing(float Healing, AActor* Healer) override;

	UFUNCTION(BlueprintImplementableEvent, Category="Combat")
	void ReceivedDamage(float Damage, const FVector& ImpactPoint, const FVector& DamageDirection);
	UPROPERTY(EditAnywhere, Category="Combat")
	UAnimMontage* DeathMontage;
	
	UPROPERTY(ReplicatedUsing=OnRep_IsDead, BlueprintReadOnly, Category="Combat")
	bool bIsDead = false;

	UPROPERTY(EditAnywhere, Category="Effects")
	float HitFlashDuration = 0.1f;

	UPROPERTY(EditAnywhere, Category="Effects")
	int32 HitFlashCount = 3;

	int32 CurrentFlashCount = 0;
	bool bFlashOn = false;
	FTimerHandle FlashTimerHandle;
	TArray<UMaterialInterface*> OriginalMaterials;

	UPROPERTY(EditAnywhere, Category="Effects")
	UMaterialInterface* HitFlashMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Boss")
	bool bIsBoss = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Boss", meta=(EditCondition="bIsBoss"))
	FText BossName;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Boss", meta=(EditCondition="bIsBoss"))
	TSubclassOf<UBossHUD> BossHUDWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category="Boss")
	UBossHUD* BossHUDWidgetInstance;
private:
	FTimerHandle DeathTimerHandle;

	UFUNCTION()
	void OnDeathTimerExpired();
	UFUNCTION()
	void OnRep_IsDead();
};
