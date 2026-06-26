// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CompanionHintWidget.h"
#include "Game3dPlayerController.generated.h"

class UInputMappingContext;
class UUserWidget;

UCLASS(abstract)
class AGame3dPlayerController : public APlayerController
{
	GENERATED_BODY()
    
public:
	UFUNCTION(BlueprintCallable, Category = "Companion Hint")
	void RequestShowHint(const FText& HintText);

	UFUNCTION(BlueprintCallable, Category = "Companion Hint")
	void RequestHideHint();

	UFUNCTION(BlueprintCallable, Category = "Game Over")
	void ShowGameOver();

	UFUNCTION(BlueprintCallable, Category = "Game Over")
	void HideGameOver();

	UFUNCTION(BlueprintCallable, Category = "Game Over")
	void RequestRespawn();


protected:
	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;

	/** Mobile controls widget to spawn */
	UPROPERTY(EditAnywhere, Category="Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	/** Pointer to the mobile controls widget */
	TObjectPtr<UUserWidget> MobileControlsWidget;

	UPROPERTY(BlueprintReadOnly, Category = "Companion Hint")
	TObjectPtr<UCompanionHintWidget> CompanionHintWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Companion Hint")
	TSubclassOf<UCompanionHintWidget> CompanionHintWidgetClass;

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UPROPERTY(EditDefaultsOnly, Category = "Game Over")
	TSubclassOf<UUserWidget> GameOverWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> GameOverWidget;


private:
	UFUNCTION(Server, Reliable)
	void Server_ShowHint(const FText& HintText);

	UFUNCTION(Server, Reliable)
	void Server_HideHint();

	UFUNCTION(Client, Reliable)
	void Client_ShowHint(const FText& HintText);

	UFUNCTION(Client, Reliable)
	void Client_HideHint();
	
	UFUNCTION(Server, Reliable)
	
	void Server_Respawn();
	
	void Server_Respawn_Implementation();

	UFUNCTION(Client, Reliable)
	void Client_HideGameOverAfterRespawn();
};