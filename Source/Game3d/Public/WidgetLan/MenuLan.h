// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "MenuLan.generated.h"

/**
 * 
 */
UCLASS()
class GAME3D_API UMenuLan : public UUserWidget
{
	GENERATED_BODY()

public:
virtual void NativeOnInitialized() override;
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Hostear;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Unirse;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UEditableTextBox> EditableTextBox_DirrecionIP;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> Level;

	UFUNCTION()
	void OnButtonHostearClicked();

	UFUNCTION()
	void OnButtonUnirseClicked();
};
