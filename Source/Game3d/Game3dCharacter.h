// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "CombatAttacker.h"
#include "CombatDamageable.h"
#include "HandleHit.h"
#include "UpdateMission.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Public/HealthBar.h"
#include "Public/XpBar.h"
#include "WidgetMedkit/UWMedkitHUD.h"
#include "GemCarrierInterface.h"
#include "Game3dCharacter.generated.h"

class AGemItem;
class USpecialAbilityHUD;
class UNiagaraSystem;
class UMotionLinesWidget;
class UMissionWidget;
class UMiniMapWidget;
class ANpc;
class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;
class UAnimMontage;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class AGame3dCharacter : public ACharacterBase, public ICombatAttacker, public ICombatDamageable, public IGemCarrierInterface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
protected:
    UFUNCTION(Server, Reliable)
    void Server_ClimbingMove(float Right, float Forward);
	
	/** Jump InputAction */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* ClimbAction;
	// Sensor del punto más alto del salto
	virtual void NotifyJumpApex() override;

	/** Run Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* RunAction;
	
	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;

	/** Dash Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* DashAction;

	/** Dash Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* UseMedkitAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* AdvanceDialogueAction;

	/** Distance ahead of the character that melee attack sphere collision traces will extend */
	UPROPERTY(EditAnywhere, Category="Melee Attack|Trace", meta = (ClampMin = 0, ClampMax = 500, Units="cm"))
	float MeleeTraceDistance = 75.0f;

	/** Radius of the sphere trace for melee attacks */
	UPROPERTY(EditAnywhere, Category="Melee Attack|Trace", meta = (ClampMin = 0, ClampMax = 200, Units = "cm"))
	float MeleeTraceRadius = 75.0f;

	/** Amount of damage a melee attack will deal */
	UPROPERTY(EditAnywhere, Category="Attack|Damage", meta = (ClampMin = 0, ClampMax = 100))
	float MeleeDamage = 45.0f;

	/** Amount of damage a melee attack will deal */
	UPROPERTY(EditAnywhere, Category="Attack|Damage", meta = (ClampMin = 0, ClampMax = 100), BlueprintReadWrite)
	float DistanceDamage = 45.0f;

	/** Knockback horizontal */
	UPROPERTY(EditAnywhere, Category="Melee Attack|Damage", meta = (ClampMin = 0, ClampMax = 1000, Units = "cm/s"))
	float MeleeKnockbackImpulse = 15.0f;

	/** Knockback vertical */
	UPROPERTY(EditAnywhere, Category="Melee Attack|Damage", meta = (ClampMin = 0, ClampMax = 1000, Units = "cm/s"))
	float MeleeLaunchImpulse = 20.0f;
public:

	/** Constructor */
	AGame3dCharacter();

protected:
	virtual void BeginPlay() override;

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	void ExecuteSpecialAbility();
	/** Called for movement input */
	void Move(const FInputActionValue& Value);
	void StopMove(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for dash input */
	void Dash();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	bool bHasUnlockedSpecialAbility = false;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* PickUpAction;
	
	UFUNCTION(BlueprintImplementableEvent, Category="Combat")
	void DealtDamage(float Damage, const FVector& ImpactPoint);
	
	// --- De ICombatDamageable ---
	virtual void ApplyDamage(float Damage, AActor* DamageCauser, const FVector& DamageLocation, const FVector& DamageImpulse) override;
	virtual void ApplyHealing(float Healing, AActor* Healer) override;
	UFUNCTION(BlueprintImplementableEvent, Category="Combat")
	void ReceivedDamage(float Damage, const FVector& ImpactPoint, const FVector& DamageDirection);
public:
	
	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles dash inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoDash();
	void PlayDashFXLocal();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();
	void OnAdvanceDialogue();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

	UFUNCTION(BlueprintCallable)
	virtual void EquipItem(FItemStruct ItemData);

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoUseMedkit();
	
	UFUNCTION()
	void HandleXpChanged(float Xp, float MaxXp);

	UFUNCTION()
	void HandleLevelChanged(int level);

	virtual void HandleLifeChanged(float Health, float MaxHealth) override;
	
	virtual void HandleDeath() override;
	/** Called from a delegate when the dash montage ends */
	void DashMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	FOnMontageEnded OnDashMontageEnded;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UXpComponent* XpComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UInventoryComponent* InventoryComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UHealthBar> HealthBarWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UXpBar> XpBarWidgetClass;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ShowGameOver();
	void Multicast_ShowGameOver_Implementation();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_HideGameOver();
	void Multicast_HideGameOver_Implementation();

	
	/** Clase del HUD de botiquines (Blueprint derivado de UWMedkitHUD) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUWMedkitHUD> MedkitHUDClass;

	/** Instancia actual del HUD mostrada en pantalla */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UUWMedkitHUD* MedkitHUDInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	TSubclassOf<USpecialAbilityHUD> SpecialAbilityHUDClass;
 
	UPROPERTY()
	USpecialAbilityHUD* SpecialAbilityHUDInstance;
	
	UPROPERTY()
	UXpBar* XpWidget;

	UPROPERTY()
	UMiniMapWidget* MiniMapWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UMiniMapWidget> MinimapWidgetClass;

	UPROPERTY()
	UMissionWidget* MissionWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UMissionWidget> MissionWidgetClass;

	UPROPERTY()
	UMotionLinesWidget* MotionLinesWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UMotionLinesWidget> MotionLinesWidgetClass;

	UPROPERTY()
	UStaticMeshComponent* EquippedMesh = nullptr;

	// Para evitar golpear varias veces al mismo actor en un ataque
	TArray<AActor*> HitActors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	float WalkSpeed = 500.f;
   
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	float SprintSpeed = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	float DashSpeed = 1200.f;
	
	int32 MedkitCount = 0;

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void UnlockSpecialAbility();

	/** Combo Attack Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* ComboAttackAction;

	/** Combo attack montage */
	UPROPERTY(EditAnywhere, Category = "Combat|Combo")
	UAnimMontage* ComboAttackMontage;

	/** Sections (stages) of the combo */
	UPROPERTY(EditAnywhere, Category = "Combat|Combo")
	TArray<FName> ComboSectionNames;

	/** Combo input tolerance (time between hits) */
	UPROPERTY(EditAnywhere, Category = "Combat|Combo", meta = (ClampMin = 0.1f, ClampMax = 2.0f, Units = "s"))
	float ComboInputCacheTimeTolerance = 0.3f;

	/** Cached time of last combo input */
	float CachedAttackInputTime = 0.0f;

	/** If true, currently attacking */

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsInDialogue = false;

	ANpc* CurrentNpc;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Audio")
	USoundBase* PickupSound;
	
	/** Current combo index */
	int32 ComboCount = 0;

	void ComboAttackPressed();

	/** Performs combo attack */
	void ComboAttack();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayComboAttack();
	
	UFUNCTION(Server, Reliable)
	void Server_SetAttackWalkSpeed();
	
	UFUNCTION(Server, Reliable)
	void Server_JumpToComboSection(int32 SectionIndex);
	void Server_JumpToComboSection_Implementation(int32 SectionIndex);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_JumpToComboSection(int32 SectionIndex);
	void Multicast_JumpToComboSection_Implementation(int32 SectionIndex);

	UFUNCTION(Server, Reliable)
	void Server_ComboAttack();
	/** Called when montage ends */
	void AttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	/** Check combo continuation */
	void CheckCombo();

	UFUNCTION(BlueprintCallable)
	void ForceStopSprintIfRunning();
	void CheckInteractable();
	AActor* FindInteractableActor();

	FOnMontageEnded OnAttackMontageEnded;

	
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
		// Funcion blueprint-callable para realizar un salto extra
    UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoPickUp();
	void AddItemToInventory(const FItemStruct& ItemData);

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoStartSprint();
   
    UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoStopSprint();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float NormalArmLength = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float SprintArmLength = 550.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float ArmInterpolationSpeed = 5.f; // Ajustable

	/** Handles combo attack pressed from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoComboAttackStart();
	UFUNCTION(Server, Reliable)
	void Server_SetCachedAttackInputTime(float Time);
	/** Handles combo attack released from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoComboAttackEnd();

	void Tick(float DeltaTime);

	
	/** Passes control to Blueprint to enable or disable jump trails */
	UFUNCTION(BlueprintImplementableEvent, Category = "Platforming")
	void SetJumpTrailState(bool bEnabled);

	UFUNCTION(BlueprintCallable)
	void HandleCraftMedkit();
	
	virtual void DoAttackTrace(FName DamageSourceBone) override;

	virtual void CheckChargedAttack() override;

	/** Ends the dash state */
	void EndDash();

	// ---- Variables ----
	int32 JumpCount = 0;
	uint8 bHasDashed : 1;

	// La gravedad pesada de tu juego
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravedad")
	float GravedadNormal = 2.0f;

	// La gravedad más suave para caer lento después del salto
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravedad")
	float GravedadCaidaLenta = 3.5f;
	
	UPROPERTY(EditAnywhere, Category = "Dash")
	UAnimMontage* DashMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump|Suspension")
	int32 MaxJumpCount = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump|Suspension")
	float SuspensionDuration = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump|Suspension")
	float SuspensionSpeed = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump|Suspension")
	bool bUseInputDirection = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump|Suspension")
	float EndLiftVelocityZ = -200.f;

	// Fall Damage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fall Damage", meta = (ClampMin = "0.0", ClampMax = "5000.0", UIMin = "0.0", UIMax = "5000.0"))
	float SafeFallSpeed = 1600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fall Damage", meta = (ClampMin = "0.0", ClampMax = "5000.0", UIMin = "0.0", UIMax = "5000.0"))
	float LethalFallSpeed = 3000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fall Damage", meta = (ClampMin = "0.0", ClampMax = "1000.0", UIMin = "0.0", UIMax = "200.0"))
	float MinFallDamage = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fall Damage", meta = (ClampMin = "0.0", ClampMax = "1000.0", UIMin = "0.0", UIMax = "200.0"))
	float MaxFallDamage = 100.f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Movement")
	float MoveLeftRightAxis = 0.f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Movement")
	float MoveUpDownAxis = 0.f;

	UFUNCTION(Server, Unreliable)
	void Server_UpdateMoveAxis(float Right, float Forward);


	UPROPERTY(ReplicatedUsing=OnRep_IsDead, BlueprintReadOnly, Category="Combat")
	bool bIsDead = false;

	UPROPERTY(EditAnywhere, Category="Combat")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, Category="Combat")
	float DeathRespawnDelay = 3.f;

	UFUNCTION()
	void OnRep_IsDead();

	FTimerHandle DeathTimerHandle;

	UFUNCTION()
	void OnDeathTimerExpired();
	
	UPROPERTY()
	AActor* CurrentInteractableActor = nullptr;
	// ---- Funciones ----
	virtual void Landed(const FHitResult& Hit) override;

	UFUNCTION(BlueprintImplementableEvent)
	void ShowInteractMessage(FName ItemName);

	UFUNCTION(BlueprintImplementableEvent)
	void HideInteractMessage();

	UPROPERTY(Replicated,BlueprintReadOnly)
	bool bIsClimbing = false;

	UPROPERTY(Replicated,BlueprintReadOnly)
	uint8 bIsDashing : 1;

	UPROPERTY(ReplicatedUsing=OnRep_IsAiming, BlueprintReadOnly)
	bool bIsAiming = false;

	UPROPERTY(Replicated,BlueprintReadOnly)
	bool bIsResting = false;

	UPROPERTY(Replicated,BlueprintReadOnly)
	bool bIsAttacking = false;

	UFUNCTION(BlueprintCallable)
	void SetClimbing(bool bNewClimbing);

	UFUNCTION(Server, Reliable)
	void Server_SetClimbing(bool bNewClimbing);

	
	UFUNCTION(BlueprintCallable)
	void SetAiming(bool bNewAiming);

	UFUNCTION(Server, Reliable)
	void Server_SetAiming(bool bNewAiming);
	
	UFUNCTION(BlueprintCallable)
	void SetResting(bool bNewResting);

	UFUNCTION(Server, Reliable)
	void Server_SetResting(bool bNewResting);

	UFUNCTION()
	void OnRep_IsResting();

	// AGame3dCharacter.h - agregar:

	UPROPERTY(ReplicatedUsing = OnRep_IsSprinting)
	bool bIsSprinting = false;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fall Damage", meta = (ClampMin = "0.0", ClampMax = "1000.0", UIMin = "0.0", UIMax = "200.0"))
	bool bIsJumpingLaunchpad = false;

	UFUNCTION()
	void OnRep_IsSprinting();

	UFUNCTION()
	void OnRep_IsAiming();
	
	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnRestingChanged(bool bNewResting);

	UFUNCTION(Server, Reliable)
	void Server_SetSprinting(bool bNewSprinting);

	UFUNCTION(Server, Reliable)
	void Server_DoDash();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayDashFX();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void StartIdleCheck();
    
    protected:
        /** Tiempo sin moverse para entrar en resting */
        UPROPERTY(EditDefaultsOnly, Category = "Resting")
        float IdleTimeToRest = 30.f;
    
    private:
        /** Handle del timer de idle */
        FTimerHandle IdleTimerHandle;
    
        /** Acumula tiempo quieto */
        float IdleElapsedTime = 0.f;
    
        /** Tick de chequeo (cada 0.1s para no sobrecargar) */
        void IdleTick();

	// ─── Resting Montages ─────────────────────────────────────────
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Resting|Montages")
	UAnimMontage* RestingEnterMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Resting|Montages")
	UAnimMontage* RestingExitMontage;

	UPROPERTY(BlueprintReadOnly)
	float AimPitch;

	UPROPERTY(BlueprintReadOnly)
	float AimYaw;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	USoundBase* DashSound;

	UFUNCTION()
	void HandleMissionUpdated(const FString& NewMission);

	UFUNCTION(Server, Reliable)
	void Server_Interact(AActor* Target);
	
// ============================================================
//  AGREGA ESTO EN Game3dCharacter.h
//  Dentro de la clase AGame3dCharacter
// ============================================================

// ── Includes nuevos (arriba del .h, junto a los demás) ──────
// #include "Engine/HitResult.h"         // ya suele estar incluido
// #include "GameFramework/ProjectileMovementComponent.h"  // en BP_Bullet

// ============================================================
//  SECCIÓN: DISPARO (Fire System)
// ============================================================

// ---------- Input ----------
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
TObjectPtr<UInputAction> FireAction;

// ---------- Assets asignados en BP ----------
/** Montage que se reproduce al disparar */
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Fire")
TObjectPtr<UAnimMontage> FireMontage;

/** Sonido 2D al disparar */
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Fire")
TObjectPtr<USoundBase> FireSound;

/** Sistema de partículas que se spawnea en el socket "gun" */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Fire")
	TObjectPtr<UNiagaraSystem> MuzzleParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Fire")
	TObjectPtr<UNiagaraSystem> ImpactParticle;

/** Clase del proyectil a spawnear */
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Fire")
TSubclassOf<AActor> BulletClass;


/** Delay entre disparos (Do Once cooldown) */
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Fire")
float FireCooldown = 0.5f;

// ---------- Estado interno ----------
bool bCanFire = true;
FTimerHandle FireCooldownTimer;

// ---------- Funciones públicas ----------
public:

	// Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> SpecialAbilityAction;

	// Cooldown
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|SpecialAbility")
	float SpecialAbilityCooldown = 15.f;

	// Para el HUD
	UFUNCTION(BlueprintCallable, Category = "Combat|SpecialAbility")
	float GetSpecialAbilityCooldownRemaining() const
	{
		if (bCanUseSpecialAbility) return 0.f;
		return GetWorldTimerManager().GetTimerRemaining(SpecialAbilityCooldownTimer);
	}
	UPROPERTY(ReplicatedUsing = OnRep_CanUseSpecialAbility)
	bool bCanUseSpecialAbility = true;
	UFUNCTION()
	void OnRep_CanUseSpecialAbility();
private:
	FTimerHandle SpecialAbilityCooldownTimer;	
/** Llamado por el input IA_Fire */
void Fire();

// ---------- RPCs ----------
protected:

/** Cliente → Servidor: solicita el disparo */
UFUNCTION(Server, Reliable)
void Server_Fire(FVector TraceStart, FVector TraceEnd);
void Server_Fire_Implementation(FVector TraceStart, FVector TraceEnd);

/** Servidor → Todos: reproduce efectos visuales / audio */
UFUNCTION(NetMulticast, Unreliable)
void Multicast_FireFX(FVector MuzzleLocation, FVector ImpactPoint, bool bHit);
void Multicast_FireFX_Implementation(FVector MuzzleLocation, FVector ImpactPoint, bool bHit);

// ---------- Helpers privados ----------
private:

/** Construye el rayo desde el centro del viewport */
void GetFireTraceVectors(FVector& OutStart, FVector& OutEnd) const;

/** Lógica de daño + spawn proyectil — solo en servidor */
void ProcessFireOnServer(const FVector& TraceStart, const FVector& TraceEnd);
private:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayRestingEnter();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayRestingExit();

	UFUNCTION()
	void OnAnyKeyPressed();

	bool bIsExitingRest = false;

	FTimerHandle SpecialAbilityDamageTimer;
	FTimerHandle SpecialAbilityEndTimer;
	int32        SpecialAbilityTickCount  = 0;
	int32        SpecialAbilityMaxTicks   = 0;
 
	void SpecialAbilityTick();   // se llama cada TickInterval
	void SpecialAbilityEnd();    // se llama al terminar la duración
 
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_SpecialAbilityFX(FVector Location);
	void Multicast_SpecialAbilityFX_Implementation(FVector Location);
 
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_SpecialAbilityEnd();
	void Multicast_SpecialAbilityEnd_Implementation();
public:
	void UseSpecialAbility();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|SpecialAbility")
	float SpecialAbilityKnockbackForce = 30000.f;  // fuerza horizontal

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|SpecialAbility")
	float SpecialAbilityLaunchForce = 10000.f;     // fuerza vertical (hacia arriba)
	UFUNCTION(Server, Reliable)
	void Server_UseSpecialAbility();
	void Server_UseSpecialAbility_Implementation();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|SpecialAbility")
	TObjectPtr<UNiagaraSystem> SpecialAbilityFX;
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|SpecialAbility")
	TObjectPtr<USoundBase> SpecialAbilitySound;
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|SpecialAbility")
	TObjectPtr<UAnimMontage> SpecialAbilityMontage;
 
	// ── Configuración del área ───────────────────────────────────
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|SpecialAbility")
	float SpecialAbilityRadius = 400.f;
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|SpecialAbility")
	float SpecialAbilityDamagePerTick = 15.f;   // daño cada tick (cada 0.5s → 4 ticks en 2s)
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|SpecialAbility")
	float SpecialAbilityDuration = 2.f;
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|SpecialAbility")
	float SpecialAbilityTickInterval = 0.5f;    // cada cuántos segundos hace daño
	
 
	UPROPERTY(EditDefaultsOnly, Category = "Climbing")
	UAnimMontage* ClimbingIdleMontage;   // climbing_idle_3
 
	// ---------- Funciones ----------
 
	 void TryStartClimbing();
 
	UFUNCTION(BlueprintCallable, Category = "Climbing")
	void StopClimbing();
 
	bool ClimbingLineTrace(FHitResult& OutHit);

	// Server RPCs
	UFUNCTION(Server, Reliable)
	void Server_StartClimbing(FRotator WallRotation);

	UFUNCTION(Server, Reliable)
	void Server_StopClimbing();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayClimbingMontage();

	// Helpers
	void StartClimbingOnServer(FRotator WallRotation);
	void StopClimbingOnServer();

	// ── Sistema de diamantes (IGemCarrierInterface) ──────────────
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gem")
	USceneComponent* GemHoldPoint;

	UPROPERTY(ReplicatedUsing = OnRep_HeldGem)
	AGemItem* HeldGem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gem", meta = (ClampMin = "0.1", ClampMax = "1.0"))
	float GemCarrySpeedMultiplier = 0.6f;

	// ── Input para tirar el diamante ─────────────────────────────
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* ThrowGemAction;

	// ── Configuración del lanzamiento ────────────────────────────
	/** Velocidad horizontal del tiro (cm/s). Unos 600–800 da "unos metros adelante" */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gem|Throw",
	          meta = (ClampMin = "100", ClampMax = "2000", Units = "cm/s"))
	float GemThrowHorizontalSpeed = 700.f;

	/** Velocidad vertical del tiro — controla el arco de la parábola */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gem|Throw",
	          meta = (ClampMin = "0", ClampMax = "1500", Units = "cm/s"))
	float GemThrowVerticalSpeed = 800.f;

	// ── Funciones ────────────────────────────────────────────────
	UFUNCTION()
	void OnRep_HeldGem();

	void RecalculateWalkSpeed();

	/** Suelta el diamante (E con gema en mano) */
	void DropGem();

	/** Lanza el diamante en parábola (input ThrowGemAction) */
	void ThrowGem();

	/** Notifica en Blueprint que el personaje soltó la gema (para sonido/VFX) */
	UFUNCTION(BlueprintImplementableEvent, Category = "Gem")
	void OnGemDropped();

	virtual USceneComponent* GetGemHoldPoint_Implementation() override;
	virtual AGemItem* GetHeldGem_Implementation() override;
	virtual void SetHeldGem_Implementation(AGemItem* NewGem) override;

	// ── RPCs ─────────────────────────────────────────────────────
	UFUNCTION(Server, Reliable)
	void Server_DropGem();

	UFUNCTION(Server, Reliable)
	void Server_ThrowGem(FVector LaunchVelocity, FVector SpawnLocation);
};
