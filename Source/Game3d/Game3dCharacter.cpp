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
#include "Public/XpComponent.h"
#include "Engine/DamageEvents.h"
#include "Game3d.h"
#include "HealthComponent.h"
#include "InventoryComponent.h"
#include "Kismet/GameplayStatics.h"

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
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

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
	// signal the character to jump
	Jump();
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
			HealthComponent->IncreaseHealth(300);

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
	HealthBarWidget->UpdateBar(Health,MaxHealth);
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

void AGame3dCharacter::DoStartSprint()
{
	if (!bIsAttacking && !bIsAiming){
	bIsSprinting = true;
	GetCharacterMovement()->MaxWalkSpeed = 900.f; // o el valor que quieras
	}
}

void AGame3dCharacter::DoStopSprint()
{
	bIsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = 500.f; // velocidad normal
}

void AGame3dCharacter::DoDash()
{
	// ignore the input if we've already dashed and have yet to reset
	if (bHasDashed)
		return;

	// raise the dash flags
	bIsDashing = true;
	bHasDashed = true;

	// disable gravity while dashing
	GetCharacterMovement()->GravityScale = 0.0f;

	// reset the character velocity so we don't carry momentum into the dash
	GetCharacterMovement()->Velocity = FVector::ZeroVector;

	// enable the jump trails
	SetJumpTrailState(true);

	// play the dash montage
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		const float MontageLength = AnimInstance->Montage_Play(DashMontage, 1.5f, EMontagePlayReturnType::MontageLength, 0.0f, true);

		// has the montage played successfully?
		if (MontageLength > 0.0f)
		{
			AnimInstance->Montage_SetEndDelegate(OnDashMontageEnded, DashMontage);
		}
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
}

void AGame3dCharacter::DashMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
		EndDash();
}

void AGame3dCharacter::EndDash()
{
	// restore gravity
	GetCharacterMovement()->GravityScale = NormalGravityScale;
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
	if(!bIsResting){
	// route the input
	DoComboAttackStart();
	}
}

void AGame3dCharacter::DoComboAttackStart()
{
	// are we already playing an attack animation?
	if (bIsAttacking)
	{
		// cache the input time so we can check it later
		CachedAttackInputTime = GetWorld()->GetTimeSeconds();

		return;
	}
	// perform a combo attack
	ComboAttack();
}

void AGame3dCharacter::DoComboAttackEnd()
{
	// stub
}

void AGame3dCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

	// raise the attacking flag
	bIsAttacking = true;

	// reset the combo count
	ComboCount = 0;

	// play the attack montage
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		const float MontageLength = AnimInstance->Montage_Play(ComboAttackMontage, 1.0f, EMontagePlayReturnType::MontageLength, 0.0f, true);

		// subscribe to montage completed and interrupted events
		if (MontageLength > 0.0f)
		{
			// set the end delegate for the montage
			AnimInstance->Montage_SetEndDelegate(OnAttackMontageEnded, ComboAttackMontage);
		}
	}
}

void AGame3dCharacter::AttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bIsAttacking = false;

	if (GetWorld()->GetTimeSeconds() - CachedAttackInputTime <= ComboInputCacheTimeTolerance)
	{
		ComboAttack();
	}
}

void AGame3dCharacter::CheckCombo()
{
	if (bIsAttacking)
	{
		if (GetWorld()->GetTimeSeconds() - CachedAttackInputTime <= ComboInputCacheTimeTolerance)
		{
			CachedAttackInputTime = 0.0f;
			
			++ComboCount;

			if (ComboCount < ComboSectionNames.Num())
			{

				if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
				{
					AnimInstance->Montage_JumpToSection(ComboSectionNames[ComboCount], ComboAttackMontage);
				}
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