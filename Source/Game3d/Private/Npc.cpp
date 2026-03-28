
#include "Npc.h"
#include "TimerManager.h"
#include "GameFramework/Actor.h"
#include "DialogueLine.h" // o el nombre real donde está FDialogueLine
#include "DialogueWidget.h"
#include "Blueprint/UserWidget.h"

ANpc::ANpc()
{
	PrimaryActorTick.bCanEverTick = false;

	// Crear trigger
	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(RootComponent);
	InteractionSphere->SetSphereRadius(200.f);
}

void ANpc::BeginPlay()
{
	Super::BeginPlay();

	InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &ANpc::OnPlayerEnter);
}

void ANpc::OnPlayerEnter(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (bIsInDialogue) return;

	if (OtherActor && OtherActor != this)
	{
		StartDialogue();
	}
}
void ANpc::StartDialogue()
{
	UE_LOG(LogTemp, Warning, TEXT("StartDialogue llamado"));

	if (!DialogueTable)
	{
		UE_LOG(LogTemp, Error, TEXT("DialogueTable es NULL"));
		return;
	}

	if (StartRow.IsNone())
	{
		UE_LOG(LogTemp, Error, TEXT("StartRow es NONE"));
		return;
	}

	if (bIsInDialogue)
	{
		UE_LOG(LogTemp, Warning, TEXT("Ya estaba en dialogo"));
		return;
	}

	bIsInDialogue = true;
	CurrentRow = StartRow;

	UE_LOG(LogTemp, Warning, TEXT("Dialogo iniciado. Row: %s"), *CurrentRow.ToString());

	// Validar clase del widget
	if (!DialogueWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("DialogueWidgetClass es NULL"));
		bIsInDialogue = false;
		return;
	}

	// Crear widget si no existe
	if (!DialogueWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("Creando DialogueWidget..."));

		DialogueWidget = CreateWidget<UDialogueWidget>(GetWorld(), DialogueWidgetClass);

		if (!DialogueWidget)
		{
			UE_LOG(LogTemp, Error, TEXT("Fallo al crear el DialogueWidget"));
			bIsInDialogue = false;
			return;
		}

		UE_LOG(LogTemp, Warning, TEXT("Widget creado correctamente"));
		DialogueWidget->AddToViewport();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Widget ya existia, reutilizando"));
	}

	// Seguridad extra
	if (!DialogueWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("DialogueWidget sigue siendo NULL despues de crearlo"));
		bIsInDialogue = false;
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Llamando a ShowCurrentLine"));
	ShowCurrentLine();
}

void ANpc::ShowCurrentLine()
{
	if (!DialogueTable) return;

	FDialogueLine* Row = DialogueTable->FindRow<FDialogueLine>(CurrentRow, "");

	if (!Row)
	{
		UE_LOG(LogTemp, Warning, TEXT("Row no encontrada, terminando dialogo"));
		EndDialogue();
		bIsInDialogue = false;
		return;
	}

	// 👇 ACA ESTABA EL BUG
	if (DialogueWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("Mostrando texto: %s"), *Row->Text.ToString());
		DialogueWidget->StartDialogueLine(*Row);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("DialogueWidget es NULL"));
	}

	// Si termina acá
	if (Row->bEndDialogue)
	{
		GetWorld()->GetTimerManager().SetTimer(
			DialogueTimer,
			[this]()
			{
				bIsInDialogue = false;
				EndDialogue();
			},
			Row->Duration,
			false
		);

		return;
	}

	// Avanzar automáticamente
	GetWorld()->GetTimerManager().SetTimer(
		DialogueTimer,
		this,
		&ANpc::AdvanceDialogue,
		Row->Duration,
		false
	);
}

void ANpc::AdvanceDialogue()
{
	FDialogueLine* Row = DialogueTable->FindRow<FDialogueLine>(CurrentRow, "");

	if (!Row) return;

	CurrentRow = Row->NextRow;
	ShowCurrentLine();
}