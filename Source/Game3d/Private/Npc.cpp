
#include "Npc.h"
#include "TimerManager.h"
#include "GameFramework/Actor.h"
#include "DialogueLine.h" // o el nombre real donde está FDialogueLine
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
	if (!DialogueTable || StartRow.IsNone()) return;

	bIsInDialogue = true;
	CurrentRow = StartRow;

	// Crear widget si no existe
	if (DialogueWidgetClass && !DialogueWidget)
	{
		DialogueWidget = CreateWidget<UUserWidget>(GetWorld(), DialogueWidgetClass);
		if (DialogueWidget)
		{
			DialogueWidget->AddToViewport();
		}
	}

	ShowCurrentLine();
}
void ANpc::ShowCurrentLine()
{
	if (!DialogueTable) return;

	FDialogueLine* Row = DialogueTable->FindRow<FDialogueLine>(CurrentRow, "");

	if (!Row)
	{
		EndDialogue();
		bIsInDialogue = false;
		return;
	}

	// Mostrar en UI (Blueprint)
	DisplayDialogue(*Row);

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