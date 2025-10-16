// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetLan/MenuLan.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Kismet/GameplayStatics.h"

void UMenuLan::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	Button_Hostear->OnClicked.AddDynamic(this,&UMenuLan::OnButtonHostearClicked);
	Button_Unirse->OnClicked.AddDynamic(this,&UMenuLan::OnButtonUnirseClicked);
	FInputModeUIOnly InputMode;
	GetOwningPlayer()->SetInputMode(InputMode);
	GetOwningPlayer()->SetShowMouseCursor(true);
}

void UMenuLan::OnButtonHostearClicked()
{
	FInputModeGameOnly InputMode;
	GetOwningPlayer()->SetInputMode(InputMode);
	GetOwningPlayer()->SetShowMouseCursor(false);
	UGameplayStatics::OpenLevelBySoftObjectPtr(this,Level,true,TEXT("listen"));
}

void UMenuLan::OnButtonUnirseClicked()
{
	FInputModeGameOnly InputMode;
	GetOwningPlayer()->SetInputMode(InputMode);
	GetOwningPlayer()->SetShowMouseCursor(false);

	const FString IP = EditableTextBox_DirrecionIP->GetText().ToString();
	UGameplayStatics::OpenLevel(this,*IP);
}
