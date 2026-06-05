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
void UMissionWidget::FadeWidget(UWidget* Widget, bool bFadeIn, float Duration, TFunction<void()> OnComplete)
{
	if (!Widget) return;

	float StartAlpha = bFadeIn ? 0.f : 1.f;
	float EndAlpha = bFadeIn ? 1.f : 0.f;

	Widget->SetRenderOpacity(StartAlpha);
	Widget->SetVisibility(ESlateVisibility::Visible);

	const int32 Steps = 30;
	const float StepTime = Duration / Steps;
	const float AlphaStep = (EndAlpha - StartAlpha) / Steps;

	TSharedPtr<int32> CurrentStep = MakeShared<int32>(0);
	TSharedPtr<FTimerHandle> Handle = MakeShared<FTimerHandle>();

	TWeakObjectPtr<UWidget> WeakWidget(Widget);
	TWeakObjectPtr<UMissionWidget> WeakThis(this);

	GetWorld()->GetTimerManager().SetTimer(
		*Handle,
		[WeakThis, WeakWidget, CurrentStep, Steps, StartAlpha, AlphaStep, EndAlpha, Handle, OnComplete, bFadeIn]()
		{
			if (!WeakThis.IsValid() || !WeakWidget.IsValid())
				return;

			(*CurrentStep)++;

			float NewAlpha = StartAlpha + AlphaStep * (*CurrentStep);
			NewAlpha = FMath::Clamp(NewAlpha, 0.f, 1.f);

			WeakWidget->SetRenderOpacity(NewAlpha);

			if (*CurrentStep >= Steps)
			{
				WeakThis->GetWorld()->GetTimerManager().ClearTimer(*Handle);

				WeakWidget->SetRenderOpacity(EndAlpha);

				if (!bFadeIn)
				{
					WeakWidget->SetVisibility(ESlateVisibility::Hidden);
				}

				if (OnComplete)
				{
					OnComplete();
				}
			}
		},
		StepTime,
		true);
}

void UMissionWidget::SetMissionText(const FString& NewMission)
{
	if (!CurrentMissionText || !MissionTitleText)
		return;

	CurrentMissionText->SetText(FText::FromString(NewMission));

	FadeWidget(MissionTitleText, true, 0.5f);
	FadeWidget(CurrentMissionText, true, 0.5f);

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_HideMission);

	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle_HideMission,
		[this]()
		{
			FadeWidget(MissionTitleText, false, 0.5f);
			FadeWidget(CurrentMissionText, false, 0.5f);
		},
		3.0f,
		false);
}