#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DialogueLine.generated.h"

USTRUCT(BlueprintType)
struct FDialogueLine : public FTableRowBase
{
	GENERATED_BODY()

	// Texto de la línea de diálogo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dialogue")
	FText Text;

	// Duración en pantalla (si no usás input para avanzar)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dialogue")
	float Duration = 2.0f;

	// Nombre del NPC que habla
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dialogue")
	FName SpeakerName = NAME_None;

	// Retrato opcional del NPC
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dialogue")
	UTexture2D* Portrait = nullptr;

	// ID de la siguiente línea (para branching simple o secuencial)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dialogue")
	FName NextRow = NAME_None;

	// Si esta línea termina el diálogo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dialogue")
	bool bEndDialogue = false;
};