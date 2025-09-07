// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "XpBar.generated.h"
class UProgressBar;
class UTextBlock;
/**
 * 
 */
UCLASS()
class GAME3D_API UXpBar : public UUserWidget
{
	GENERATED_BODY()
	
public:

    /** Actualiza la barra de XP */
    UFUNCTION(BlueprintCallable, Category = "XP")
    void UpdateXpBar(float CurrentXP, float MaxXP);

    /** Actualiza el texto del nivel */
    UFUNCTION(BlueprintCallable, Category = "XP")
    void UpdateLevelText(int32 Level);


protected:

    /** Referencia al ProgressBar de la UI */
    UPROPERTY(meta = (BindWidget))
    UProgressBar* XpProgressBar;

    /** Texto para mostrar XP (ej: 20/100) */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* XpNumber;

    /** Texto para mostrar el nivel */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* LevelNumber;
};
