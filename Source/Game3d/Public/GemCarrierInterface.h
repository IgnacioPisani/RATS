#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GemCarrierInterface.generated.h"

class AGemItem;

// Cualquier Pawn/Character que pueda llevar un diamante implementa esta interfaz.
// Así el AGemItem y el AGemPedestal le preguntan al Interactor sin hacer Cast a tu clase de personaje.
UINTERFACE(BlueprintType)
class UGemCarrierInterface : public UInterface
{
	GENERATED_BODY()
};

class IGemCarrierInterface
{
	GENERATED_BODY()

public:
	// Punto (component) a la altura del cuerpo donde se attachea el diamante mientras se lo lleva
	UFUNCTION(BlueprintNativeEvent, Category = "Gem")
	USceneComponent* GetGemHoldPoint();

	// Diamante que tiene agarrado ahora mismo (nullptr si no tiene ninguno)
	UFUNCTION(BlueprintNativeEvent, Category = "Gem")
	AGemItem* GetHeldGem();

	// Para que el diamante/soporte le actualicen la referencia al personaje
	UFUNCTION(BlueprintNativeEvent, Category = "Gem")
	void SetHeldGem(AGemItem* NewGem);
};
