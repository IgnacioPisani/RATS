// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetMedkit/UWMedkitHUD.h"
#include "Components/ProgressBar.h"


void UUWMedkitHUD::UpdateMedkitBars(int32 CurrentMedkits)
{
	if (!Bar_Medkit1 || !Bar_Medkit2 || !Bar_Medkit3)
	{
		UE_LOG(LogTemp, Warning, TEXT("UWMedkitHUD: Falta vincular una o más ProgressBars en el widget Blueprint."));
		return;
	}

	Bar_Medkit1->SetVisibility(CurrentMedkits >= 1 ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	Bar_Medkit2->SetVisibility(CurrentMedkits >= 2 ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	Bar_Medkit3->SetVisibility(CurrentMedkits >= 3 ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

	UE_LOG(LogTemp, Log, TEXT("HUD actualizado: %d botiquines."), CurrentMedkits);
}