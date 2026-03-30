#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogueLine.h"
#include "DialogueWidget.generated.h"

class UTextBlock;

UCLASS()
class GAME3D_API UDialogueWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	// 🔹 Widgets
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TxtName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TxtDialogue;

	// 🔹 Función principal
	void StartDialogueLine(const FDialogueLine& Line);

	void StopTyping();
	
	bool bIsTyping = false;

	UFUNCTION(BlueprintCallable)
	bool IsTyping() const { return bIsTyping; }

protected:

	virtual void NativeConstruct() override;

private:

	// 🔹 Texto completo
	FString FullText;

	// 🔹 Índice actual
	int32 CurrentCharIndex;

	// 🔹 Timer
	FTimerHandle TypingTimer;

	// 🔹 Velocidad
	UPROPERTY(EditAnywhere)
	float TypingSpeed = 0.03f;

	// 🔹 Función interna
	void TypeNextCharacter();
	
};