// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueWidget.h"

#include "Components/TextBlock.h"

void UDialogueWidget::StartDialogueLine(const FDialogueLine& Line)
{
	if (!TxtDialogue) return;

	// Limpiar timer anterior
	GetWorld()->GetTimerManager().ClearTimer(TypingTimer);
	bIsTyping = true;
	// Resetear
	FullText = Line.Text.ToString();
	CurrentCharIndex = 0;

	// Nombre (hardcode o podés pasarlo después)
	if (TxtName)
	{
		TxtName->SetText(FText::FromString("NPC"));
	}

	// Limpiar texto
	TxtDialogue->SetText(FText::FromString(""));

	// Empezar animación
	GetWorld()->GetTimerManager().SetTimer(
		TypingTimer,
		this,
		&UDialogueWidget::TypeNextCharacter,
		TypingSpeed,
		true
	);
}

// Add default functionality here for any IDialogueWidget functions that are not pure virtual.
void UDialogueWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CurrentCharIndex = 0;
}

void UDialogueWidget::TypeNextCharacter()
{
	if (!TxtDialogue) return;

	if (CurrentCharIndex >= FullText.Len())
	{
		// Terminar animación
		GetWorld()->GetTimerManager().ClearTimer(TypingTimer);
		bIsTyping = false;

		return;
	}

	CurrentCharIndex++;
	FString SubText = FullText.Left(CurrentCharIndex);
	TxtDialogue->SetText(FText::FromString(SubText));
}

void UDialogueWidget::StopTyping()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(TypingTimer);
	}
	// Mostrar texto completo (opcional)
	TxtDialogue->SetText(FText::FromString(FullText));
	bIsTyping = false;

}
