#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine/DataTable.h"
#include "Components/SphereComponent.h"
#include "Npc.generated.h"

class AGame3dCharacter;
class UDialogueWidget;
struct FDialogueLine;

UCLASS()
class GAME3D_API ANpc : public ACharacter
{
	GENERATED_BODY()

public:
	ANpc();
	void HandleAdvanceInput();

protected:
	virtual void BeginPlay() override;

	// 🔵 Trigger de interacción
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USphereComponent* InteractionSphere;

	// 🧠 DataTable de diálogo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dialogue")
	UDataTable* DialogueTable;

	// 🔰 Fila inicial
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dialogue")
	FName StartRow;

	// 🔄 Fila actual
	FName CurrentRow;

	// ⏱ Timer
	FTimerHandle DialogueTimer;

	AGame3dCharacter* MyChar;


	// 🔐 Estado
	bool bIsInDialogue = false;

	bool bHasSpoken = false;
	
	// 🧩 Funciones
	UFUNCTION()
	void OnPlayerEnter(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	void StartDialogue();
	void ShowCurrentLine();
	void AdvanceDialogue();
	 

	// 🖥️ Mostrar en UI (lo implementás en BP si querés)
	UFUNCTION(BlueprintImplementableEvent, Category="Dialogue")
	void DisplayDialogue(const FDialogueLine& DialogueData);

	UFUNCTION()
	void EndDialogue();

	// Widget class (set desde Blueprint)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UDialogueWidget> DialogueWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDialogueWidget* DialogueWidget;
};