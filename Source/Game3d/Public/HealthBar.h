// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBar.generated.h"

UCLASS()
class GAME3D_API UHealthBar : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void UpdateBar(float Health, float MaxHealth);

    UFUNCTION(BlueprintCallable)
    void SetBarColor(const FLinearColor& NewColor);
protected:
    UPROPERTY(meta = (BindWidget),BlueprintReadWrite)
    class UProgressBar* HealthProgressBar;
};
