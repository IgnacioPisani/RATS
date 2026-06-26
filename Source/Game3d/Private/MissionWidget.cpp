// Fill out your copyright notice in the Description page of Project Settings.


#include "MissionWidget.h"

bool UMissionWidget::Initialize()
{
	bool bSuccess = Super::Initialize();

	if (MissionTitleText)
	{
		MissionTitleText->SetText(FText::FromString(TEXT("Mission")));
		MissionTitleText->SetVisibility(ESlateVisibility::Hidden);
	}

	if (CurrentMissionText)
	{
		CurrentMissionText->SetVisibility(ESlateVisibility::Hidden);
	}

	return bSuccess;
}
void UMissionWidget::FadeWidget(UWidget* Widget, bool bFadeIn, float Duration,
								FTimerHandle& OutHandle, TFunction<void()> OnComplete)
{
	if (!Widget || !GetWorld()) return;

	// Cancelá cualquier fade en curso sobre este widget
	GetWorld()->GetTimerManager().ClearTimer(OutHandle);

	float StartAlpha  = bFadeIn ? 0.f : 1.f;
	float EndAlpha    = bFadeIn ? 1.f : 0.f;

	Widget->SetRenderOpacity(StartAlpha);
	Widget->SetVisibility(ESlateVisibility::Visible);

	const int32 Steps     = 30;
	const float StepTime  = Duration / Steps;
	const float AlphaStep = (EndAlpha - StartAlpha) / Steps;

	TSharedPtr<int32> CurrentStep = MakeShared<int32>(0);
	TWeakObjectPtr<UWidget>        WeakWidget(Widget);
	TWeakObjectPtr<UMissionWidget> WeakThis(this);

	GetWorld()->GetTimerManager().SetTimer(
		OutHandle,
		[WeakThis, WeakWidget, CurrentStep, Steps, StartAlpha, AlphaStep, EndAlpha, bFadeIn, OnComplete]()
		{
			if (!WeakThis.IsValid() || !WeakWidget.IsValid()) return;

			(*CurrentStep)++;
			float NewAlpha = FMath::Clamp(StartAlpha + AlphaStep * (*CurrentStep), 0.f, 1.f);
			WeakWidget->SetRenderOpacity(NewAlpha);

			if (*CurrentStep >= Steps)
			{
				WeakWidget->SetRenderOpacity(EndAlpha);
				if (!bFadeIn)
					WeakWidget->SetVisibility(ESlateVisibility::Hidden);
				if (OnComplete)
					OnComplete();
			}
		},
		StepTime, true);
}

void UMissionWidget::SetMissionText(const FString& NewMission)
{
	if (!CurrentMissionText || !MissionTitleText || !GetWorld()) return;

	CurrentMissionText->SetText(FText::FromString(NewMission));

	FadeWidget(MissionTitleText,   true, 0.5f, TimerHandle_TitleFade);
	FadeWidget(CurrentMissionText, true, 0.5f, TimerHandle_MissionFade);

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_HideMission);
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle_HideMission,
		[this]()
		{
			FadeWidget(MissionTitleText,   false, 0.5f, TimerHandle_TitleFade);
			FadeWidget(CurrentMissionText, false, 0.5f, TimerHandle_MissionFade);
		},
		3.0f, false);
}