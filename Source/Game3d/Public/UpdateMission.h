// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UpdateMission.generated.h"


UINTERFACE(MinimalAPI)
class UUpdateMission : public UInterface
{
	GENERATED_BODY()
};

class GAME3D_API IUpdateMission
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateMission(const FString& CurrentMission);
};