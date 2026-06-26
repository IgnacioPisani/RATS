#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CompanionHintInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UCompanionHintInterface : public UInterface
{
	GENERATED_BODY()
};

class GAME3D_API ICompanionHintInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Companion Hint")
	void ShowHint(const FText& HintText);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Companion Hint")
	void HideHint();
};