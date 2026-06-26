// Copyright Epic Games, Inc. All Rights Reserved.

#include "Game3dPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "Game3d.h"
#include "Widgets/Input/SVirtualJoystick.h"
#include "GameFramework/GameModeBase.h"
#include "CompanionHintWidget.h"

void AGame3dPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // Mobile controls
    if (SVirtualJoystick::ShouldDisplayTouchInterface() && IsLocalPlayerController())
    {
        MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

        if (MobileControlsWidget)
        {
            MobileControlsWidget->AddToPlayerScreen(0);
        }
        else
        {
            UE_LOG(LogGame3d, Error, TEXT("Could not spawn mobile controls widget."));
        }
    }

    // Companion hint widget
    if (IsLocalController() && CompanionHintWidgetClass)
    {
        CompanionHintWidget = CreateWidget<UCompanionHintWidget>(this, CompanionHintWidgetClass);
        if (CompanionHintWidget)
        {
            CompanionHintWidget->AddToViewport(10);
            CompanionHintWidget->HideHint();  // directo, sin interfaz
        }
    }
}

void AGame3dPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (IsLocalPlayerController())
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
        {
            for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
            {
                Subsystem->AddMappingContext(CurrentContext, 0);
            }

            if (!SVirtualJoystick::ShouldDisplayTouchInterface())
            {
                for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
                {
                    Subsystem->AddMappingContext(CurrentContext, 0);
                }
            }
        }
    }
}

void AGame3dPlayerController::RequestShowHint(const FText& HintText)
{
    if (HasAuthority())
        Client_ShowHint(HintText);
    else
        Server_ShowHint(HintText);
}

void AGame3dPlayerController::RequestHideHint()
{
    if (HasAuthority())
        Client_HideHint();
    else
        Server_HideHint();
}

void AGame3dPlayerController::Server_ShowHint_Implementation(const FText& HintText)
{
    Client_ShowHint(HintText);
}

void AGame3dPlayerController::Server_HideHint_Implementation()
{
    Client_HideHint();
}

void AGame3dPlayerController::Client_ShowHint_Implementation(const FText& HintText)
{
    if (CompanionHintWidget)
        CompanionHintWidget->ShowHint(HintText);
}

void AGame3dPlayerController::Client_HideHint_Implementation()
{
    if (CompanionHintWidget)
        CompanionHintWidget->HideHint();
}

void AGame3dPlayerController::ShowGameOver()
{
    if (GameOverWidgetClass && !GameOverWidget)
    {
        GameOverWidget = CreateWidget<UUserWidget>(this, GameOverWidgetClass);
    }

    if (GameOverWidget)
    {
        GameOverWidget->AddToViewport(20);
        bShowMouseCursor = true;
        SetInputMode(FInputModeUIOnly());
    }
}

void AGame3dPlayerController::HideGameOver()
{
    if (GameOverWidget)
    {
        GameOverWidget->RemoveFromParent();
        bShowMouseCursor = false;
        SetInputMode(FInputModeGameOnly());
    }
}

void AGame3dPlayerController::RequestRespawn()
{
    Server_Respawn();
}

void AGame3dPlayerController::Server_Respawn_Implementation()
{
    AActor* SpawnPoint = GetWorld()->GetAuthGameMode()->FindPlayerStart(this);

    if (GetPawn())
    {
        GetPawn()->Destroy();
    }

    GetWorld()->GetAuthGameMode()->RestartPlayerAtPlayerStart(this, SpawnPoint);

    Client_HideGameOverAfterRespawn();
}

// Agregá este RPC también:
// En el .h dentro de private:
// UFUNCTION(Client, Reliable)
// void Client_HideGameOverAfterRespawn();

void AGame3dPlayerController::Client_HideGameOverAfterRespawn_Implementation()
{
    HideGameOver();
}