#include "CompanionHintWidget.h"
#include "Components/TextBlock.h"
#include "Components/Overlay.h"

void UCompanionHintWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Text_HintMessage)
	{
		// Wrap automático — ajustá el ancho a lo que uses en el widget
		Text_HintMessage->SetAutoWrapText(true);
	}
}

void UCompanionHintWidget::ShowHint(const FText& HintText)
{
	if (Overlay_Root)
		Overlay_Root->SetVisibility(ESlateVisibility::HitTestInvisible);

	if (Anim_SlideIn)
		PlayAnimation(Anim_SlideIn);

	// Resetear estado del typewriter
	FullText = HintText.ToString();
	CurrentCharIndex = 0;

	if (Text_HintMessage)
		Text_HintMessage->SetText(FText::GetEmpty());

	// Cancelar timer anterior si había uno corriendo
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TypewriterTimer);
		World->GetTimerManager().SetTimer(
			TypewriterTimer,
			this,
			&UCompanionHintWidget::TypeNextChar,
			TypewriterSpeed,
			true  // loop
		);
	}
}

void UCompanionHintWidget::HideHint()
{
	// Cancelar typewriter si se oculta antes de terminar
	if (UWorld* World = GetWorld())
		World->GetTimerManager().ClearTimer(TypewriterTimer);

	if (Overlay_Root)
		Overlay_Root->SetVisibility(ESlateVisibility::Collapsed);
}

void UCompanionHintWidget::TypeNextChar()
{
	if (CurrentCharIndex >= FullText.Len())
	{
		// Terminó, paramos el timer
		if (UWorld* World = GetWorld())
			World->GetTimerManager().ClearTimer(TypewriterTimer);
		return;
	}

	// Agregamos un carácter más al texto visible
	FString Visible = FullText.Left(++CurrentCharIndex);

	if (Text_HintMessage)
		Text_HintMessage->SetText(FText::FromString(Visible));
}