// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueWidget.h"

#include "Components/TextBlock.h"

// Add default functionality here for any IDialogueWidget functions that are not pure virtual.
void UDialogueWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CurrentCharIndex = 0;
}

void UDialogueWidget::StartDialogueLine(const FDialogueLine& Line)
{
	if (!TxtDialogue) return;

	// Limpiar timer anterior
	GetWorld()->GetTimerManager().ClearTimer(TypingTimer);

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

void UDialogueWidget::TypeNextCharacter()
{
	if (!TxtDialogue) return;

	if (CurrentCharIndex >= FullText.Len())
	{
		// Terminar animación
		GetWorld()->GetTimerManager().ClearTimer(TypingTimer);
		return;
	}

	CurrentCharIndex++;

	FString SubText = FullText.Left(CurrentCharIndex);
	TxtDialogue->SetText(FText::FromString(SubText));
}