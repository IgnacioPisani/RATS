// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BossHUD.generated.h"

/**
 * 
 */
UCLASS()
class GAME3D_API UBossHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	// Actualiza la barra de vida del jefe
	UFUNCTION(BlueprintCallable, Category="Boss")
	void UpdateBar(float CurrentHealth, float MaxHealth);

	// Setea el nombre del jefe (se llama una vez al iniciar)
	UFUNCTION(BlueprintCallable, Category="Boss")
	void SetBossName(const FText& Name);

protected:
	// Implementado en el Widget Blueprint: actualiza el progress bar visualmente
	UFUNCTION(BlueprintImplementableEvent, Category="Boss")
	void OnHealthUpdated(float Percent);

	// Implementado en el Widget Blueprint: setea el texto del nombre
	UFUNCTION(BlueprintImplementableEvent, Category="Boss")
	void OnNameSet(const FText& Name);
};