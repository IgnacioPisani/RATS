// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "CombatAttacker.h"
#include "HandleHit.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Public/HealthBar.h"
#include "Public/XpBar.h"
#include "Game3dCharacter.generated.h"

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
class AGame3dCharacter : public ACharacterBase, public ICombatAttacker
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
protected:

	/** Jump InputAction */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

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

public:

	/** Constructor */
	AGame3dCharacter();	

protected:
	virtual void BeginPlay() override;

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for dash input */
	void Dash();

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* PickUpAction;
	
	
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

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

	UFUNCTION(BlueprintCallable)
	virtual void EquipItem(FItemStruct ItemData);
	
	
	UFUNCTION()
	void HandleXpChanged(float Xp, float MaxXp);

	UFUNCTION()
	void HandleLevelChanged(int level);

	virtual void HandleLifeChanged(float Health, float MaxHealth) override;
	
	virtual void HandleDeath() override;
	
	virtual void HandleHit_Implementation() override;

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

	UPROPERTY()
	UXpBar* XpWidget;

	UPROPERTY()
	UStaticMeshComponent* EquippedMesh = nullptr;

	// Para evitar golpear varias veces al mismo actor en un ataque
	TArray<AActor*> HitActors;

   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
   float WalkSpeed = 500.f;
   
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
   float SprintSpeed = 800.f;
   
   bool bIsSprinting = false;

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
	float ComboInputCacheTimeTolerance = 0.45f;

	/** Cached time of last combo input */
	float CachedAttackInputTime = 0.0f;

	/** If true, currently attacking */
	bool bIsAttacking = false;

	/** Current combo index */
	int32 ComboCount = 0;

	void ComboAttackPressed();

	/** Performs combo attack */
	void ComboAttack();

	/** Called when montage ends */
	void AttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	/** Check combo continuation */
	void CheckCombo();

	FOnMontageEnded OnAttackMontageEnded;

	
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
		// Funcion blueprint-callable para realizar un salto extra
    UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoPickUp();

    UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoStartSprint();
   
    UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoStopSprint();

	/** Handles combo attack pressed from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoComboAttackStart();

	/** Handles combo attack released from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoComboAttackEnd();

	/** Passes control to Blueprint to enable or disable jump trails */
	UFUNCTION(BlueprintImplementableEvent, Category = "Platforming")
	void SetJumpTrailState(bool bEnabled);

	virtual void DoAttackTrace(FName DamageSourceBone) override;

	virtual void CheckChargedAttack() override;

	/** Ends the dash state */
	void EndDash();

	// ---- Variables ----
	int32 JumpCount = 0;
	uint8 bHasDashed : 1;
	uint8 bIsDashing : 1;
	
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
	float NormalGravityScale = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump|Suspension")
	float EndLiftVelocityZ = -200.f;

	// ---- Funciones ----
	virtual void Landed(const FHitResult& Hit) override;

};