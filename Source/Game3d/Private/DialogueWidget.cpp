// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueWidget.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"

void UDialogueWidget::StartDialogueLine(const FDialogueLine& Line)
{
	if (!TxtDialogue) return;

	// Limpiar timer anterior
	GetWorld()->GetTimerManager().ClearTimer(TypingTimer);
	bIsTyping = true;
	// Resetear
	FullText = Line.Text.ToString();
	CurrentCharIndex = 0;

	// Nombre del speaker — tomado del struct en vez de hardcodeado
	if (TxtName)
	{
		if (Line.SpeakerName != NAME_None)
		{
			TxtName->SetText(FText::FromName(Line.SpeakerName));
		}
		else
		{
			TxtName->SetText(FText::GetEmpty());
		}
	}

	// Retrato del speaker
	if (ImgPortrait)
	{
		if (Line.Portrait)
		{
			ImgPortrait->SetBrushFromTexture(Line.Portrait);
			ImgPortrait->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			// Sin retrato para esta línea — ocultamos la imagen en vez de
			// dejar el brush anterior pegado o un cuadro vacío visible.
			ImgPortrait->SetVisibility(ESlateVisibility::Hidden);
		}
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

	if (TxtDialogue)
	{
		// Mostrar texto completo (opcional)
		TxtDialogue->SetText(FText::FromString(FullText));
	}

	bIsTyping = false;
}