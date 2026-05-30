// ============================================================
//  SpecialAbilityHUD.h  — copiá en tu carpeta Public/
// ============================================================
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpecialAbilityHUD.generated.h"

class AGame3dCharacter;
class UProgressBar;
class UTextBlock;
class UImage;

UCLASS()
class GAME3D_API USpecialAbilityHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Llamalo desde BeginPlay del personaje para enlazar el widget */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetOwnerCharacter(AGame3dCharacter* Character);

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// ── Widgets que bindás vos en el BP del widget ───────────
	
	/** Barra de cooldown (0 = listo, 1 = en cooldown) */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> CooldownBar;

	/** Texto con los segundos restantes, ej: "12.3s" */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> CooldownText;

	/** Ícono de la habilidad — lo podés oscurecer cuando está en cooldown */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UImage> AbilityIcon;

private:
	UPROPERTY()
	TObjectPtr<AGame3dCharacter> OwnerCharacter;

	/** Actualiza barra, texto e ícono cada tick */
	void UpdateCooldownUI(float Remaining, float Total);
};