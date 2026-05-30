// ============================================================
//  SpecialAbilityHUD.cpp
// ============================================================
#include "SpecialAbilityHUD.h"

#include "Game3dCharacter.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void USpecialAbilityHUD::SetOwnerCharacter(AGame3dCharacter* Character)
{
	OwnerCharacter = Character;
}

void USpecialAbilityHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!OwnerCharacter) return;

	const float Remaining = OwnerCharacter->GetSpecialAbilityCooldownRemaining();
	const float Total     = OwnerCharacter->SpecialAbilityCooldown;

	UpdateCooldownUI(Remaining, Total);
}

void USpecialAbilityHUD::UpdateCooldownUI(float Remaining, float Total)
{
	const bool bReady = Remaining <= 0.f;

	// ── Barra ────────────────────────────────────────────────
	// 0 = listo (barra vacía / llena según tu diseño)
	// 1 = cooldown completo recién activado
	if (CooldownBar)
	{
		const float Percent = bReady ? 0.f : (Remaining / Total);
		CooldownBar->SetPercent(Percent);
	}

	// ── Texto ────────────────────────────────────────────────
	if (CooldownText)
	{
		if (bReady)
		{
			CooldownText->SetText(FText::FromString(TEXT("Q")));
		}
		else
		{
			// Muestra "12.3s"
			CooldownText->SetText(
				FText::FromString(
					FString::Printf(TEXT("%.1fs"), Remaining)
				)
			);
		}
	}

	// ── Ícono: opaco cuando listo, semitransparente en cooldown ─
	if (AbilityIcon)
	{
		const float Alpha = bReady ? 1.f : 0.4f;
		AbilityIcon->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, Alpha));
	}
}