// ============================================================
//  SpecialAbilityHUD.h
// ============================================================
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpecialAbilityHUD.generated.h"

class AGame3dCharacter;
class UTextBlock;
class UImage;
class UMaterialInterface;
class UMaterialInstanceDynamic;

UCLASS()
class GAME3D_API USpecialAbilityHUD : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void SetOwnerCharacter(AGame3dCharacter* Character);

protected:
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    // ── Bindings (nombres exactos en el WBP) ─────────────────

    /** Image con el material radial de la serpiente */
    UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
    TObjectPtr<UImage> CooldownImage;

    /** Ícono central de la habilidad — siempre visible */
    UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
    TObjectPtr<UImage> AbilityIcon;

    /** Texto de segundos restantes — fade in/out */
    UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
    TObjectPtr<UTextBlock> CooldownText;

    // ── Configuración ─────────────────────────────────────────

    /** Material radial (M_RoundProgressbar) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD|Style")
    TObjectPtr<UMaterialInterface> RoundProgressMaterial;

    /** Velocidad del fade del texto y transición del ícono */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD|Animation")
    float FadeSpeed = 4.f;

    /** Brillo del ícono cuando está en cooldown (0=negro, 1=normal) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD|Style",
        meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float DisabledBrightness = 0.35f;

    // ── Eventos Blueprint ─────────────────────────────────────

    /** Llamado cuando termina el cooldown — para animación de "lista" */
    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void OnAbilityReady();

    /** Llamado cuando se activa la habilidad */
    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void OnAbilityActivated();

    
private:
    UPROPERTY()
    TObjectPtr<AGame3dCharacter> OwnerCharacter;

    UPROPERTY()
    TObjectPtr<UMaterialInstanceDynamic> CooldownMaterialInstance;

    float TextOpacity           = 0.f;
    float CurrentIconBrightness = 1.f;  // arranca en normal (listo)
    bool  bWasOnCooldown        = false;

    void UpdateCooldownUI(float Remaining, float Total, float DeltaTime);

    
};