// Fill out your copyright notice in the Description page of Project Settings.

#include "XpBar.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
void UXpBar::UpdateXpBar(float CurrentXP, float MaxXP)
{
    if (XpProgressBar && MaxXP > 0.f)
    {
        float Percent = CurrentXP / MaxXP;
        XpProgressBar->SetPercent(Percent);
    }

    if (XpNumber)
    {
        FText XpText = FText::Format(
            FText::FromString("{0}/{1}"),
            FText::AsNumber(FMath::FloorToInt(CurrentXP)),
            FText::AsNumber(FMath::FloorToInt(MaxXP))
        );

        XpNumber->SetText(XpText);
    }
}

void UXpBar::UpdateLevelText(int32 Level)
{
    if (LevelNumber)
    {
        LevelNumber->SetText(FText::AsNumber(Level));
    }
}
