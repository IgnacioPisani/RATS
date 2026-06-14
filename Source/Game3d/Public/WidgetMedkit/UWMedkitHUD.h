// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "UWMedkitHUD.generated.h"

/**
 * 
 */
UCLASS()
class GAME3D_API UUWMedkitHUD : public UUserWidget
{
	GENERATED_BODY()
    
    public:
    
    	// Actualiza las barritas según la cantidad actual de botiquines
    	UFUNCTION(BlueprintCallable, Category = "MedkitHUD")
    	void UpdateMedkitBars(int32 CurrentMedkits);
    
    protected:
    
    	// Referencias a las ProgressBars (se asignan en el editor de UMG o en C++)
    	UPROPERTY(meta = (BindWidgetOptional))
    	UProgressBar* Bar_Medkit1;
    
    	UPROPERTY(meta = (BindWidgetOptional))
    	UProgressBar* Bar_Medkit2;
    
    	UPROPERTY(meta = (BindWidgetOptional))
    	UProgressBar* Bar_Medkit3;
    
    private:
    
    	// Colores predefinidos
    	FLinearColor ColorActive = FLinearColor(0.f, 0.479f, 0.875f, 1.0f); // Verde
};
