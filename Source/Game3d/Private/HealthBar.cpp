// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthBar.h"
#include "Components/ProgressBar.h"


void UHealthBar::UpdateBar(float Health, float MaxHealth)
{
	HealthProgressBar->SetPercent(Health / MaxHealth);
}

void UHealthBar::SetBarColor(const FLinearColor& NewColor)
{
	if (!HealthProgressBar) return;

	HealthProgressBar->SetFillColorAndOpacity(NewColor);
}