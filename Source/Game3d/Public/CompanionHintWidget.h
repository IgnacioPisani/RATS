#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CompanionHintWidget.generated.h"

class UTextBlock;
class UOverlay;

UCLASS()
class GAME3D_API UCompanionHintWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Companion Hint")
	void ShowHint(const FText& HintText);

	UFUNCTION(BlueprintCallable, Category = "Companion Hint")
	void HideHint();

	// Velocidad en segundos entre cada carácter
	UPROPERTY(EditDefaultsOnly, Category = "Companion Hint")
	float TypewriterSpeed = 0.04f;

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_HintMessage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UOverlay> Overlay_Root;

	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> Anim_SlideIn;

private:
	void TypeNextChar();

	FString FullText;
	int32 CurrentCharIndex;
	FTimerHandle TypewriterTimer;
};