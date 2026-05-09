// Copyright Epic Games, Inc. All Rights Reserved.

#include "Game3dCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Public/HealthBar.h"
#include "Interactable.h"  
#include "Public/XpBar.h"
#include "Public/MiniMapWidget.h"
#include "Public/XpComponent.h"
#include "Engine/DamageEvents.h"
#include "Game3d.h"
#include "HealthComponent.h"
#include "InventoryComponent.h"
#include "MissionWidget.h"
#include "Npc.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"
AGame3dCharacter::AGame3dCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	//Dash 
	bHasDashed = false;
	bIsDashing = false;

	// bind the attack montage ended delegate
	OnDashMontageEnded.BindUObject(this, &AGame3dCharacter::DashMontageEnded);
	OnAttackMontageEnded.BindUObject(this, &AGame3dCharacter::AttackMontageEnded);

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 800.f;
	GetCharacterMovement()->AirControl = 0.65f;
	GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->MaxWalkSpeed = 50.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 10.f;
	
	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	XpComponent = CreateDefaultSubobject<UXpComponent>(TEXT("XpComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AGame3dCharacter::BeginPlay()
{
	Super::BeginPlay();
	SetCanBeDamaged(true);
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	if (HasAuthority())
	{
		StartIdleCheck();
	}
	if (MissionWidgetClass)
	{
		// Crear el widget
		MissionWidget = CreateWidget<UMissionWidget>(GetWorld(), MissionWidgetClass);

		if (MissionWidget)
		{
			MissionWidget->AddToViewport();
		}
	}
	if (!IsLocallyControlled()) return;
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->InputComponent->BindKey(
			EKeys::AnyKey,
			IE_Pressed,
			this,
			&AGame3dCharacter::OnAnyKeyPressed
		);
	}
	if (HealthBarWidgetClass)
	{
		HealthBarWidget = CreateWidget<UHealthBar>(GetWorld(), HealthBarWidgetClass);
		if (HealthBarWidget)
		{
			HealthBarWidget->AddToViewport();
		}
	}
	if (XpComponent)
	{
		// Suscripci�n a los eventos del componente
		XpComponent->OnXpChanged.AddDynamic(this, &AGame3dCharacter::HandleXpChanged);
		XpComponent->OnLevelChanged.AddDynamic(this, &AGame3dCharacter::HandleLevelChanged);
	} 
 	if (XpBarWidgetClass)
	{
		// Crear el widget
		XpWidget = CreateWidget<UXpBar>(GetWorld(), XpBarWidgetClass);

		if (XpWidget)
		{
			XpWidget->AddToViewport();
		}
	}
	if (MinimapWidgetClass)
	{
		// Crear el widget
		MiniMapWidget = CreateWidget<UMiniMapWidget>(GetWorld(), MinimapWidgetClass);

		if (MiniMapWidget)
		{
			MiniMapWidget->AddToViewport();
		}
	}

	if (MedkitHUDClass)
	{
		MedkitHUDInstance = CreateWidget<UUWMedkitHUD>(GetWorld(), MedkitHUDClass);
		if (MedkitHUDInstance)
		{
			MedkitHUDInstance->AddToViewport();
			MedkitHUDInstance->UpdateMedkitBars(0);
		}
	}
}

void AGame3dCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGame3dCharacter::Move);
		EnhancedInputComponent->BindAction(
	MoveAction,
	ETriggerEvent::Completed,
	this,
	&AGame3dCharacter::StopMove
);

EnhancedInputComponent->BindAction(
	MoveAction,
	ETriggerEvent::Canceled,
	this,
	&AGame3dCharacter::StopMove
);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AGame3dCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGame3dCharacter::Look);
		EnhancedInputComponent->BindAction(PickUpAction, ETriggerEvent::Started, this, &AGame3dCharacter::DoPickUp);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &AGame3dCharacter::DoStartSprint);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &AGame3dCharacter::DoStopSprint);

		EnhancedInputComponent->BindAction(ComboAttackAction, ETriggerEvent::Started, this, &AGame3dCharacter::ComboAttackPressed);
		EnhancedInputComponent->BindAction(UseMedkitAction, ETriggerEvent::Started, this, &AGame3dCharacter::DoUseMedkit);

		EnhancedInputComponent->BindAction(AdvanceDialogueAction, ETriggerEvent::Started, this, &AGame3dCharacter::OnAdvanceDialogue);

	}
	else
	{
		UE_LOG(LogGame3d, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AGame3dCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}
void AGame3dCharacter::StopMove(const FInputActionValue& Value)
{
	MoveLeftRightAxis = 0.f;
	MoveUpDownAxis    = 0.f;
}
void AGame3dCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AGame3dCharacter::ApplyDamage(float Damage, AActor* DamageCauser, const FVector& DamageLocation,
	const FVector& DamageImpulse)
{
	// pass the damage event to the actor
	FDamageEvent DamageEvent;
	const float ActualDamage = TakeDamage(Damage, DamageEvent, nullptr, DamageCauser);

	// only process knockback and effects if we received nonzero damage
	if (ActualDamage > 0.0f)
	{
		// apply the knockback impulse
		GetCharacterMovement()->AddImpulse(DamageImpulse, true);

		// is the character ragdolling?
		if (GetMesh()->IsSimulatingPhysics())
		{
			// apply an impulse to the ragdoll
			GetMesh()->AddImpulseAtLocation(DamageImpulse * GetMesh()->GetMass(), DamageLocation);
		}

		// pass control to BP to play effects, etc.
		ReceivedDamage(ActualDamage, DamageLocation, DamageImpulse.GetSafeNormal());
	}
}

void AGame3dCharacter::ApplyHealing(float Healing, AActor* Healer)
{
}
void AGame3dCharacter::DoMove(float Right, float Forward)
{
	if (!Controller)
	{
		return;
	}

	// ==========================
	// GUARDAR INPUT (PARA ANIMACIONES)
	// ==========================
	MoveLeftRightAxis = Right;
	MoveUpDownAxis    = Forward;
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			1,                      // Key fija (se pisa cada frame)
			0.f,                    // 0 = un frame
			FColor::Green,
			FString::Printf(
				TEXT("Climb Input | LR: %.2f  UD: %.2f"),
				MoveLeftRightAxis,
				MoveUpDownAxis
			)
		);
	}


	// ==========================
	// MODO ESCALAR
	// ==========================
	if (bIsClimbing)
	{
		AddMovementInput(GetActorUpVector(), Forward);
		AddMovementInput(GetActorRightVector(), Right);
		return;
	}

	// ==========================
	// MODO NORMAL
	// ==========================
	const FRotator ControlRot = Controller->GetControlRotation();
	const FRotator YawRot(0.f, ControlRot.Yaw, 0.f);

	const FVector ForwardDir =
		FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);

	const FVector RightDir =
		FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDir, Forward);
	AddMovementInput(RightDir, Right);
}

void AGame3dCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AGame3dCharacter::DoJumpStart()
{
	if (bIsInDialogue) return;
	Jump();
}

void AGame3dCharacter::OnAdvanceDialogue()
{
	if (!bIsInDialogue) return;

	// Llamar al NPC activo
	if (CurrentNpc)
	{
		CurrentNpc->HandleAdvanceInput();
	}
}

void AGame3dCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

void AGame3dCharacter::EquipItem(FItemStruct ItemData)
{
	if (!ItemData.Mesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemData.Mesh es nullptr"));
		return;
	}

	// Si ya hay una malla equipada, la destruimos
	if (EquippedMesh)
	{
		EquippedMesh->DestroyComponent();
		EquippedMesh = nullptr;
		UE_LOG(LogTemp, Log, TEXT("Malla previa destruida"));
	}

	// Crear un componente de malla en runtime
	EquippedMesh = NewObject<UStaticMeshComponent>(this);
	if (EquippedMesh)
	{
		EquippedMesh->RegisterComponent();
		EquippedMesh->SetStaticMesh(ItemData.Mesh);
		EquippedMesh->AttachToComponent(
			GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			TEXT("hand_rSocket")  // tu socket en el esqueleto
		);

		UE_LOG(LogTemp, Log, TEXT("Malla adjuntada al socket WeaponSocket"));
	}
}

void AGame3dCharacter::DoUseMedkit()
{
	if (InventoryComponent && MedkitCount > 0 && HealthComponent)
	{
		// Verifica que no esté al máximo de vida
		if (HealthComponent->GetCurrentHealth() < HealthComponent->GetMaxHealth())
		{
			// Consume un botiquín del inventario
			InventoryComponent->ConsumeItem("Medkit", 1);
			MedkitCount--;

			// Cura al jugador
			HealthComponent->UpdateHealth(300);

			// Actualiza el HUD
			if (MedkitHUDInstance)
			{
				MedkitHUDInstance->UpdateMedkitBars(MedkitCount);
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("Salud completa: no puedes usar un botiquín."));
		}
	}
}

void AGame3dCharacter::HandleLifeChanged(float Health, float MaxHealth)
{
	if (IsLocallyControlled() && HealthBarWidget)
	{
		HealthBarWidget->UpdateBar(Health,MaxHealth);
	}
}


void AGame3dCharacter::HandleDeath()
{
	Destroy();
}

void AGame3dCharacter::HandleXpChanged(float Xp, float MaxXp)
{
	XpWidget->UpdateXpBar(Xp,MaxXp);
}

void AGame3dCharacter::HandleLevelChanged(int level)
{
	XpWidget->UpdateLevelText(level);
}

void AGame3dCharacter::DoPickUp()
{
	AActor* HitActor = FindInteractableActor();
	if (!HitActor) return;

	if (HitActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
	{
		FItemStruct ItemData = IInteractable::Execute_GetItem(HitActor);

		if (ItemData.Mesh)
		{
			AddItemToInventory(ItemData);
			HitActor->Destroy();
		}
		else
		{
			FItemStruct SpawnData = IInteractable::Execute_GetItemSpawn(HitActor);
			IInteractable::Execute_Interact(HitActor, this);
			if (SpawnData.Mesh)
			{
				AddItemToInventory(SpawnData);
			}

		}
	}
}

void AGame3dCharacter::AddItemToInventory(const FItemStruct& ItemData)
{
	if (InventoryComponent)
	{
		InventoryComponent->AddItem(ItemData);
	}
}

void AGame3dCharacter::DoDash()
{
	if (bHasDashed) return;
	// El cliente SOLO pide al servidor, nada más
	Server_DoDash();
}

void AGame3dCharacter::Server_DoDash_Implementation()
{
	if (bHasDashed) return;

	bIsDashing = true;
	bHasDashed = true;

	GetCharacterMovement()->GravityScale = 0.0f;
	GetCharacterMovement()->Velocity = FVector::ZeroVector;
	LaunchCharacter(GetActorForwardVector() * 1200.f, true, true);

	// El servidor llama multicast → llega a TODOS correctamente
	Multicast_PlayDashFX();
}

void AGame3dCharacter::Multicast_PlayDashFX_Implementation()
{
	// Sin ningún IsLocallyControlled() check, todos reproducen igual
	SetJumpTrailState(true);

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		const float MontageLength = AnimInstance->Montage_Play(DashMontage, 1.5f);
		if (MontageLength > 0.0f)
			AnimInstance->Montage_SetEndDelegate(OnDashMontageEnded, DashMontage);
	}
}
void AGame3dCharacter::Landed(const FHitResult& Hit)
{
    Super::Landed(Hit);
	bHasDashed = false;
    float FallVelocity = FMath::Abs(GetVelocity().Z);

    float SafeFallSpeed = 1500.f;

    float LethalFallSpeed = 2000.f;

    if (FallVelocity > SafeFallSpeed)
    {
    	if (Hit.GetActor() && Hit.GetActor()->ActorHasTag("LaunchPad"))
    	{
    		UE_LOG(LogTemp, Warning, TEXT("Landed on LaunchPad → No fall damage"));
    		return;
    	}
	    else
	    {
	    	float Damage = FMath::GetMappedRangeValueClamped(
	 FVector2D(SafeFallSpeed, LethalFallSpeed),
	 FVector2D(5.f, 100.f),
	 FallVelocity
 );

	    	UGameplayStatics::ApplyDamage(
				this,
				Damage,
				GetController(),
				this,
				nullptr
			);

	    	UE_LOG(LogTemp, Warning, TEXT("Fall damage: %f (speed: %f)"), Damage, FallVelocity);
	    }
    }
	GetCharacterMovement()->MaxWalkSpeed = 500.f; // velocidad normal

}

void AGame3dCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGame3dCharacter, bIsSprinting);
	DOREPLIFETIME(AGame3dCharacter, bIsAttacking);
	DOREPLIFETIME_CONDITION_NOTIFY(
		AGame3dCharacter,
		bIsAiming,
		COND_None,
		REPNOTIFY_Always
	);
	DOREPLIFETIME(AGame3dCharacter, bIsClimbing);
	DOREPLIFETIME(AGame3dCharacter, bIsDashing);
	DOREPLIFETIME(AGame3dCharacter, bIsResting);
}

void AGame3dCharacter::OnRep_IsAiming()
{
	if (bIsAiming)
	{
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	else
	{
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
}

void AGame3dCharacter::StartIdleCheck()
{
	IdleElapsedTime = 0.f;
	GetWorldTimerManager().SetTimer(
		IdleTimerHandle,
		this,
		&AGame3dCharacter::IdleTick,
		0.1f,   // cada 100ms
		true    // looping
	);
}

void AGame3dCharacter::IdleTick()
{
	// Solo corre en servidor
	if (!HasAuthority()) return;

	const float Speed = GetVelocity().Length();

	if (Speed > 0.1f)
	{
		// Se está moviendo — resetear contador y salir de resting
		IdleElapsedTime = 0.f;

		if (bIsResting && !bIsExitingRest)
			SetResting(false);
		return;
	}

	// Quieto — acumular tiempo
	IdleElapsedTime += 0.1f;

	if (IdleElapsedTime >= IdleTimeToRest && !bIsResting)
	{
		SetResting(true);
	}
}

void AGame3dCharacter::UpdateMission_Implementation(const FString& CurrentMission)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan,
FString::Printf(TEXT("[UpdateMission] Recibida misión: %s"), *CurrentMission));


	if (MissionWidget)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green,
			TEXT("[UpdateMission] Actualizando MissionWidget"));

		MissionWidget->SetMissionText(CurrentMission);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red,
			TEXT("[UpdateMission] MissionWidget es nullptr"));
	}
}

void AGame3dCharacter::OnAnyKeyPressed()
{
	// FIX: verificar que somos el controlador local antes de procesar
	if (!IsLocallyControlled()) return;
	if (!bIsResting || bIsExitingRest) return;

	bIsExitingRest = true; // guard inmediato en cliente

	// FIX: siempre ir por Server_SetResting para que el servidor
	// sea el único que cambia el estado — evita divergencia
 
	if (HasAuthority())
		SetResting(false);
	else
		Server_SetResting(false);
}



void AGame3dCharacter::DashMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
		EndDash();
}

void AGame3dCharacter::EndDash()
{
	// restore gravity
	GetCharacterMovement()->GravityScale = GravedadNormal;
	
	// reset the dashing flag
	bIsDashing = false;

	// are we grounded after the dash?
	if (GetCharacterMovement()->IsMovingOnGround())
	{
		// reset the dash usage flag, since we won't receive a landed event
		bHasDashed = false;

		// deactivate the jump trails
		SetJumpTrailState(false);
	}
}

void AGame3dCharacter::ComboAttackPressed()
{
	if (bIsClimbing) return;
	if(!bIsResting and !bIsInDialogue ){
	// route the input
	DoComboAttackStart();
	}
}

void AGame3dCharacter::NotifyJumpApex()
{
	// Esto es obligatorio para no romper la lógica base del salto de Unreal
	Super::NotifyJumpApex(); 

	// Acá adentro va tu lógica. 
	// Por ejemplo, si querías que el personaje caiga más rápido después del salto:
	// GetCharacterMovement()->GravityScale = 5.0f; 
}



void AGame3dCharacter::Server_ComboAttack_Implementation()
{
	ComboAttack(); // servidor ejecuta y llama Multicast
}

void AGame3dCharacter::CheckCombo()
{
	// Solo el servidor maneja la lógica del combo
	if (!HasAuthority()) return;

	UE_LOG(LogTemp, Warning, TEXT("=== CheckCombo en SERVIDOR ==="));

	if (bIsAttacking)
	{
		if (GetWorld()->GetTimeSeconds() - CachedAttackInputTime <= ComboInputCacheTimeTolerance)
		{
			CachedAttackInputTime = 0.0f;
			++ComboCount;

			UE_LOG(LogTemp, Warning, TEXT("✅ Combo avanza a sección %d de %d"),
				ComboCount, ComboSectionNames.Num());

			if (ComboCount < ComboSectionNames.Num())
			{
				Server_JumpToComboSection(ComboCount);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("❌ Combo NO avanza - TimeDiff muy alto"));
		}
	}
}

void AGame3dCharacter::DoComboAttackStart()
{
	if (bIsAttacking)
	{
		CachedAttackInputTime = GetWorld()->GetTimeSeconds();
		Server_SetCachedAttackInputTime(CachedAttackInputTime); // ← esto debe estar
		return;
	}
	Server_ComboAttack();
}

void AGame3dCharacter::Server_SetCachedAttackInputTime_Implementation(float Time)
{
	// Restar el ping aproximado para compensar la latencia
	float Ping = 0.f;
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (APlayerState* PS = PC->GetPlayerState<APlayerState>())
		{
			Ping = PS->GetPingInMilliseconds() / 1000.f; // convertir a segundos
		}
	}

	// Setear el tiempo como si hubiera llegado antes (compensando latencia)
	CachedAttackInputTime = GetWorld()->GetTimeSeconds() - Ping;

	UE_LOG(LogTemp, Warning, TEXT("CachedAttackInputTime seteado | Ping: %.3f seg"), Ping);
}

void AGame3dCharacter::Server_JumpToComboSection_Implementation(int32 SectionIndex)
{
	Multicast_JumpToComboSection(SectionIndex);
}

void AGame3dCharacter::Multicast_JumpToComboSection_Implementation(int32 SectionIndex)
{
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->Montage_JumpToSection(ComboSectionNames[SectionIndex], ComboAttackMontage);
	}
}

void AGame3dCharacter::AttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!HasAuthority()) return;

	UE_LOG(LogTemp, Warning, TEXT("AttackMontageEnded | bInterrupted: %s | ComboCount: %d | TimeDiff: %f"),
		bInterrupted ? TEXT("TRUE") : TEXT("FALSE"),
		ComboCount,
		GetWorld()->GetTimeSeconds() - CachedAttackInputTime);

	const bool bShouldContinueCombo = 
		GetWorld()->GetTimeSeconds() - CachedAttackInputTime <= ComboInputCacheTimeTolerance;

	if (bShouldContinueCombo)
	{
		UE_LOG(LogTemp, Warning, TEXT("→ Continúa combo"));
		ComboAttack();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("→ Fin de combo, reseteando"));
		bIsAttacking = false;
		ComboCount = 0; // ← asegurate que esto esté
	}
}

void AGame3dCharacter::DoComboAttackEnd()
{
	// stub
}

void AGame3dCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FRotator AimRotation = GetBaseAimRotation();

	float Pitch = AimRotation.Pitch;

	if (Pitch > 90.f)
	{
		Pitch -= 360.f;
	}

	AimPitch = Pitch;
	AimYaw = AimRotation.Yaw;
	// El spring arm NO se modifica si el personaje está apuntando
	if (CameraBoom && !bIsAiming)
	{
		const float CurrentSpeed = GetVelocity().Size();

		// NUEVO: el personaje solo sprinta si está corriendo Y está en el suelo
		const bool bCanSprint =
			bIsSprinting &&
			CurrentSpeed > 5.f &&
			GetCharacterMovement()->IsMovingOnGround();

		float TargetLength = bCanSprint ? SprintArmLength : NormalArmLength;

		float NewLength = FMath::FInterpTo(
			CameraBoom->TargetArmLength,
			TargetLength,
			DeltaTime,
			ArmInterpolationSpeed
		);

		CameraBoom->TargetArmLength = NewLength;
	}
	CheckInteractable();

}

void AGame3dCharacter::HandleCraftMedkit()
{
	if (!InventoryComponent)
{
	UE_LOG(LogTemp, Warning, TEXT("No hay InventoryComponent asignado al personaje."));
	return;
}

	FName ItemToCraft = "Medkit";

	bool bSuccess = InventoryComponent->CraftItem(ItemToCraft);

	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("Crafteo exitoso de %s."), *ItemToCraft.ToString());

		// ✅ Actualizar HUD
		if (MedkitHUDInstance)
		{
			MedkitCount = InventoryComponent->GetItemQuantityByName(ItemToCraft);
			MedkitHUDInstance->UpdateMedkitBars(MedkitCount);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Error al craftear %s."), *ItemToCraft.ToString());
	}
}

void AGame3dCharacter::DoAttackTrace(FName DamageSourceBone)
{
	if (bIsClimbing) return; // 🔥 doble seguridad

	// sweep for objects in front of the character to be hit by the attack
	TArray<FHitResult> OutHits;

	// start at the provided socket location, sweep forward
	const FVector TraceStart = GetMesh()->GetSocketLocation(DamageSourceBone);
	const FVector TraceEnd = TraceStart + (GetActorForwardVector() * MeleeTraceDistance);

	// check for pawn and world dynamic collision object types
	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);
	ObjectParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	// use a sphere shape for the sweep
	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(MeleeTraceRadius);

	// ignore self
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (GetWorld()->SweepMultiByObjectType(OutHits, TraceStart, TraceEnd, FQuat::Identity, ObjectParams, CollisionShape, QueryParams))
	{

		// iterate over each object hit
		for (const FHitResult& CurrentHit : OutHits)
		{
			// check if we've hit a damageable actor
			ICombatDamageable* Damageable = Cast<ICombatDamageable>(CurrentHit.GetActor());


			if (Damageable)
			{
				FVector Direction = CurrentHit.GetActor()->GetActorLocation() - GetActorLocation();
				Direction.Normalize();
				// Invertimos la dirección para empujar al actor golpeado en sentido contrario (alejándolo del atacante)
				const FVector Impulse = (Direction * MeleeKnockbackImpulse) + (FVector::UpVector * MeleeLaunchImpulse);
				
				// pass the damage event to the actor
				Damageable->ApplyDamage(MeleeDamage, this, CurrentHit.ImpactPoint, Impulse);

				// call the BP handler to play effects, etc.
				DealtDamage(MeleeDamage, CurrentHit.ImpactPoint);
			}
		}
	}
}

void AGame3dCharacter::CheckChargedAttack()
{
}

void AGame3dCharacter::ComboAttack()
{
	if (bIsClimbing) return;
	bIsAttacking = true;
	ComboCount = 0;
	Multicast_PlayComboAttack();
}

void AGame3dCharacter::Multicast_PlayComboAttack_Implementation()
{
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		// Limpiar delegate anterior
		FOnMontageEnded EmptyDelegate;
		AnimInstance->Montage_SetEndDelegate(EmptyDelegate, ComboAttackMontage);

		const float MontageLength = AnimInstance->Montage_Play(
			ComboAttackMontage, 1.0f,
			EMontagePlayReturnType::MontageLength, 0.0f, true
		);
		if (MontageLength > 0.0f)
		{
			if (HasAuthority())
			{
				AnimInstance->Montage_SetEndDelegate(OnAttackMontageEnded, ComboAttackMontage);
			}
		}
	}
}

void AGame3dCharacter::ForceStopSprintIfRunning()
{
	if (bIsSprinting)
	{
		DoStopSprint();
	}
}

void AGame3dCharacter::CheckInteractable()
{
	AActor* HitActor = FindInteractableActor();

	if (HitActor && HitActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
	{
		if (CurrentInteractableActor != HitActor)
		{
			CurrentInteractableActor = HitActor;

			FItemStruct ItemData = IInteractable::Execute_GetItem(HitActor);

			if (ItemData.Name != NAME_None)
			{
				ShowInteractMessage(ItemData.Name);
			}		
		}
		return;
	}

	if (CurrentInteractableActor)
	{
		CurrentInteractableActor = nullptr;
		HideInteractMessage();
	}
}

AActor* AGame3dCharacter::FindInteractableActor()
{
	const float Radius = 120.f;

	FVector Start = GetActorLocation();
	Start.Z -= 65.f;

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	TArray<AActor*> FloorActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Floor"), FloorActors);
	Params.AddIgnoredActors(FloorActors);

	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		Start,
		Start,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(Radius),
		Params
	);

	if (bHit)
	{
		return HitResult.GetActor();
	}

	return nullptr;
}

void AGame3dCharacter::SetClimbing(bool bNewClimbing)
{
	if (HasAuthority())
	{
		bIsClimbing = bNewClimbing;
	}
	else
	{
		Server_SetClimbing(bNewClimbing);
	}
}

void AGame3dCharacter::SetAiming(bool bNewAiming)
{
	if (HasAuthority())
	{
		bIsAiming = bNewAiming;
	}
	else
	{
		Server_SetAiming(bNewAiming);
	}}

void AGame3dCharacter::Server_SetClimbing_Implementation(bool bNewClimbing)
{
	bIsClimbing = bNewClimbing;
}


void AGame3dCharacter::Server_SetAiming_Implementation(bool bNewAiming)
{
	bIsAiming = bNewAiming;

	OnRep_IsAiming();
}

void AGame3dCharacter::SetResting(bool bNewResting)
{
	if (HasAuthority())
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White,
			FString::Printf(TEXT("[SetResting] bNewResting = %s"),
				bNewResting ? TEXT("TRUE") : TEXT("FALSE")));
		IdleElapsedTime = 0.f;

		bIsResting = bNewResting;
		OnRep_IsResting();

		if (bNewResting)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange,
				TEXT("[SetResting] Entrando al rest → DisableMovement"));

			Multicast_PlayRestingEnter();
			GetCharacterMovement()->DisableMovement();
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan,
				TEXT("[SetResting] Saliendo del rest → PlayRestingExit"));

			Multicast_PlayRestingExit();
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow,
			FString::Printf(TEXT("[SetResting] Cliente enviando RPC → bNewResting = %s"),
				bNewResting ? TEXT("TRUE") : TEXT("FALSE")));

		Server_SetResting(bNewResting);
	}
}

void AGame3dCharacter::Multicast_PlayRestingEnter_Implementation()
{
	if (!RestingEnterMontage) return;

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->Montage_Play(RestingEnterMontage, 1.0f);
	}
}

void AGame3dCharacter::Multicast_PlayRestingExit_Implementation()
{
	if (!RestingExitMontage) return;

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->Montage_Play(RestingExitMontage, 1.0f);

		// bloquear movimiento mientras sale de resting
		GetCharacterMovement()->DisableMovement();

		// duración del montage
		const float MontageDuration = RestingExitMontage->GetPlayLength();

		FTimerHandle TimerHandle;

		GetWorldTimerManager().SetTimer(
			TimerHandle,
			[this]()
			{
				GetCharacterMovement()->SetMovementMode(MOVE_Walking);
				bIsExitingRest = false;

			},
			MontageDuration + 0.2f, // delay extra post animación
			false
		);
	}
}

void AGame3dCharacter::Server_SetResting_Implementation(bool bNewResting)
{
	IdleElapsedTime = 0.f;
	bIsResting = bNewResting;
	OnRep_IsResting();

	if (bNewResting)
	{
		Multicast_PlayRestingEnter();
		GetCharacterMovement()->DisableMovement();
	}
	else
	{
		Multicast_PlayRestingExit();
	}
}


void AGame3dCharacter::OnRep_IsResting()
{
	
	UE_LOG(LogTemp, Warning, TEXT("OnRep_IsResting en %s: %s"), 
		IsLocallyControlled() ? TEXT("LOCAL") : TEXT("REMOTO"),
		bIsResting ? TEXT("TRUE") : TEXT("FALSE"));}

// AGame3dCharacter.cpp

void AGame3dCharacter::DoStartSprint()
{
	if (!bIsAttacking && !bIsAiming)
	{
		Server_SetSprinting(true);
	}
}

void AGame3dCharacter::DoStopSprint()
{
	Server_SetSprinting(false);
}

void AGame3dCharacter::Server_SetSprinting_Implementation(bool bNewSprinting)
{
	bIsSprinting = bNewSprinting;
	// Aplica velocidad en el servidor
	GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? SprintSpeed : WalkSpeed;
	// bIsSprinting replicado dispara OnRep en los clientes
}

void AGame3dCharacter::OnRep_IsSprinting()
{
	// Se ejecuta en cada cliente cuando recibe el valor replicado
	GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? SprintSpeed : WalkSpeed;
}