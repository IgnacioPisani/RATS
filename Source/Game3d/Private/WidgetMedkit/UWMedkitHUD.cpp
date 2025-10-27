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

	// Siempre llenas (solo cambia el color)
	Bar_Medkit1->SetPercent(1.0f);
	Bar_Medkit2->SetPercent(1.0f);
	Bar_Medkit3->SetPercent(1.0f);

	// Cambiar color según cantidad
	Bar_Medkit1->SetFillColorAndOpacity(CurrentMedkits >= 1 ? ColorActive : ColorInactive);
	Bar_Medkit2->SetFillColorAndOpacity(CurrentMedkits >= 2 ? ColorActive : ColorInactive);
	Bar_Medkit3->SetFillColorAndOpacity(CurrentMedkits >= 3 ? ColorActive : ColorInactive);

	UE_LOG(LogTemp, Log, TEXT("HUD actualizado: %d botiquines."), CurrentMedkits);
}