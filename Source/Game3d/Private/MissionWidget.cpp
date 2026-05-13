// Fill out your copyright notice in the Description page of Project Settings.


#include "MissionWidget.h"

bool UMissionWidget::Initialize()
{
	bool bSuccess = Super::Initialize();

	if (MissionTitleText)
	{
		MissionTitleText->SetText(FText::FromString(TEXT("Mission")));
	}

	return bSuccess;
}

void UMissionWidget::SetMissionText(const FString& NewMission)
{
	if (CurrentMissionText)
	{
		CurrentMissionText->SetText(FText::FromString(NewMission));
	}
}