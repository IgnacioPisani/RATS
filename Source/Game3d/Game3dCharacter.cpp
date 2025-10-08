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
#include "Game3d.h"
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
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AGame3dCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGame3dCharacter::Look);
		EnhancedInputComponent->BindAction(PickUpAction, ETriggerEvent::Started, this, &AGame3dCharacter::DoPickUp);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &AGame3dCharacter::DoStartSprint);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &AGame3dCharacter::DoStopSprint);
     
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

void AGame3dCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AGame3dCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
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

void AGame3dCharacter::HandleHit_Implementation()
{
	HitActors.Empty();

	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp) return;

	FVector SocketLocation = MeshComp->GetSocketLocation(AttackSocketName);
	FRotator SocketRotation = MeshComp->GetSocketRotation(AttackSocketName);
	FVector ForwardVector = SocketRotation.Vector();
	FVector End = SocketLocation + ForwardVector * AttackRange;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	TArray<FHitResult> OutHits;

	bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetWorld(),
		SocketLocation,
		End,
		AttackRadius,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration, // cambiar a None cuando ya funcione
		OutHits,
		true
	);

	if (bHit)
	{
		for (const FHitResult& Hit : OutHits)
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor || HitActors.Contains(HitActor))
				continue;

			HitActors.Add(HitActor);

			// Aplica daño al actor golpeado
			UGameplayStatics::ApplyDamage(
				HitActor,          
				AttackDamage,     
				GetController(),   
				this,               
				nullptr            
			);
		}
	}
}


void AGame3dCharacter::DoPickUp()
{

	const float Radius = 120.f;
	FVector Start = GetActorLocation();
	Start.Z -= 65.f;
	const FVector End = Start; 

	UE_LOG(LogTemp, Log, TEXT("Iniciando SphereTrace. Radio: %.1f | Posicion: %s"), 
		   Radius, *Start.ToString());
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	TArray<AActor*> FloorActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Floor"), FloorActors);
	Params.AddIgnoredActors(FloorActors);

	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		Start,
		End,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(Radius),
		Params
	);

	UE_LOG(LogTemp, Log, TEXT("Resultado del trace -> bHit: %s"), bHit ? TEXT("TRUE") : TEXT("FALSE"));

	if (bHit)
	{
		if (HitResult.GetActor())
		{
			AActor* HitActor = HitResult.GetActor();
			UE_LOG(LogTemp, Log, TEXT("Actor impactado: %s"), *HitActor->GetName());

			if (HitActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
			{
				UE_LOG(LogTemp, Log, TEXT("El actor implementa la interfaz UInteractable"));

				FItemStruct ItemData = IInteractable::Execute_GetItem(HitActor);
			
				UE_LOG(LogTemp, Log, TEXT("ItemData recibido -> Name: %s, Cantidad: %d"),
					   *ItemData.Name.ToString(), ItemData.Quantity);

				if (InventoryComponent)
				{
					InventoryComponent->AddItem(ItemData);
					UE_LOG(LogTemp, Log, TEXT("Item agregado al inventario"));
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("InventoryComponent es nullptr"));
				}

				HitActor->Destroy();
				UE_LOG(LogTemp, Log, TEXT("Actor destruido tras recoger item"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("El actor no implementa UInteractable"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Trace detectó impacto pero GetActor() es nullptr"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("SphereTrace no encontró ningún actor"));
	}

	
}

void AGame3dCharacter::DoStartSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = 900.f; // o el valor que quieras
}

void AGame3dCharacter::DoStopSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = 600.f; // velocidad normal
}


void AGame3dCharacter::Jump()
{
	if (bIsSuspending) return;

	JumpCount++;

	if (JumpCount < MaxJumpCount)
	{
		Super::Jump();
	}
	else
	{
		FVector InputDir = GetLastMovementInputVector();
		FVector Dir = FVector::ZeroVector;

		if (bUseInputDirection && !InputDir.IsNearlyZero())
			Dir = InputDir.GetSafeNormal();
		else
		{
			Dir = GetActorForwardVector();
			Dir.Z = 0.f;
			Dir.Normalize();
		}

		StartSuspension(Dir);
	}
}

void AGame3dCharacter::StartSuspension(const FVector& Direction)
{
	if (!GetCharacterMovement()) return;

	bIsSuspending = true;
	SuspensionDirection = Direction.GetSafeNormal();

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->GravityScale = 0.f;
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);

	FVector Vel = SuspensionDirection * SuspensionSpeed;
	Vel.Z = 0.f;
	GetCharacterMovement()->Velocity = Vel;

	GetWorldTimerManager().SetTimer(SuspensionTimerHandle, this, &AGame3dCharacter::EndSuspension, SuspensionDuration, false);
}

void AGame3dCharacter::EndSuspension()
{
	if (!GetCharacterMovement()) return;

	bIsSuspending = false;

	// Restaurar gravedad normal
	GetCharacterMovement()->GravityScale = NormalGravityScale;

	// Cambiar a modo de caída
	GetCharacterMovement()->SetMovementMode(MOVE_Falling);

	// Mantener caída vertical suave en línea recta
	FVector FallVelocity = FVector::ZeroVector;
	FallVelocity.Z = -600.f; // caída natural, sin impulso brusco
	GetCharacterMovement()->Velocity = FallVelocity;

	// Resetear contador de salto
	JumpCount = 0;
}


void AGame3dCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	JumpCount = 0;
	bIsSuspending = false;
	GetWorldTimerManager().ClearTimer(SuspensionTimerHandle);
	GetCharacterMovement()->GravityScale = NormalGravityScale;
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}
