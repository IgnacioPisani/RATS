// ============================================================
//  SpecialAbilityHUD.cpp
// ============================================================
#include "SpecialAbilityHUD.h"

#include "Game3dCharacter.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"

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

    UpdateCooldownUI(Remaining, Total, InDeltaTime);
}

void USpecialAbilityHUD::UpdateCooldownUI(float Remaining, float Total, float DeltaTime)
{
    const bool  bOnCooldown = Remaining > 0.f;
    const float Percent     = (Total > 0.f) ? (Remaining / Total) : 0.f;

    // ── Detectar transición cooldown → listo ─────────────────
    if (bWasOnCooldown && !bOnCooldown)
    {
        OnAbilityReady();
    }
    bWasOnCooldown = bOnCooldown;

    // ── Material radial ──────────────────────────────────────
    // Crear instancia dinámica una sola vez
    if (CooldownImage && RoundProgressMaterial && !CooldownMaterialInstance)
    {
        CooldownMaterialInstance = UMaterialInstanceDynamic::Create(
            RoundProgressMaterial, this);
        CooldownImage->SetBrushFromMaterial(CooldownMaterialInstance);
    }
    if (CooldownMaterialInstance)
    {
        CooldownMaterialInstance->SetScalarParameterValue(TEXT("Percent"), Percent);
    }

    // ── Texto: fade in al activarse, fade out al terminar ────
    // Solo el texto tiene fade, el widget raíz siempre visible
    if (CooldownText)
    {
        if (bOnCooldown)
        {
            // Fade in del texto
            TextOpacity = FMath::FInterpTo(TextOpacity, 1.f, DeltaTime, FadeSpeed);

            const int32 SecondsLeft = FMath::CeilToInt(Remaining);
            CooldownText->SetText(
                FText::FromString(FString::Printf(TEXT("%d"), SecondsLeft))
            );
            CooldownText->SetVisibility(ESlateVisibility::HitTestInvisible);
        }
        else
        {
            // Fade out del texto
            TextOpacity = FMath::FInterpTo(TextOpacity, 0.f, DeltaTime, FadeSpeed);

            if (TextOpacity < 0.01f)
            {
                CooldownText->SetVisibility(ESlateVisibility::Hidden);
            }
        }
        CooldownText->SetRenderOpacity(TextOpacity);
    }

    // ── Ícono: disabled (oscuro+desaturado) → normal ─────────
    // Interpolamos suavemente entre el tono disabled y el normal
    if (AbilityIcon)
    {
        // TargetBrightness: 0.35 en cooldown, 1.0 cuando está lista
        const float TargetBrightness = bOnCooldown ? DisabledBrightness : 1.f;
        CurrentIconBrightness = FMath::FInterpTo(
            CurrentIconBrightness, TargetBrightness, DeltaTime, FadeSpeed);

        AbilityIcon->SetColorAndOpacity(
            FLinearColor(
                CurrentIconBrightness,
                CurrentIconBrightness,
                CurrentIconBrightness,
                1.f   // alpha siempre 1, el ícono nunca desaparece
            )
        );
    }
}