// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "MissionWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class GAME3D_API UMissionWidget : public UUserWidget
{
	GENERATED_BODY()


public:
	virtual bool Initialize() override;

	UFUNCTION(BlueprintCallable)
	void SetMissionText(const FString& NewMission);

void FadeWidget(UWidget* Widget, bool bFadeIn, float Duration, FTimerHandle& OutHandle, TFunction<void()> OnComplete = nullptr);
protected:

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MissionTitleText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentMissionText;

	FTimerHandle TimerHandle_TitleFade;
	FTimerHandle TimerHandle_MissionFade;
	FTimerHandle TimerHandle_HideMission; };
