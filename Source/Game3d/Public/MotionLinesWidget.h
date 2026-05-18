// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MotionLinesWidget.generated.h"

/**
 * 
 */
UCLASS()
class GAME3D_API UMotionLinesWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* MotionLinesImage;
};
