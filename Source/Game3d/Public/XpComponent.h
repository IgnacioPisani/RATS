#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "XpComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnXpChanged, float, Xp, float, MaxXp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelChanged, int, Level);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAME3D_API UXpComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UXpComponent();

	// 🔥 VARIABLES REPLICADAS
	UPROPERTY(ReplicatedUsing = OnRep_Xp, EditAnywhere, BlueprintReadWrite, Category = "XpLevel")
	float Xp;

	UPROPERTY(ReplicatedUsing = OnRep_MaxXp, EditAnywhere, BlueprintReadWrite, Category = "XpLevel")
	float MaxXp;

	UPROPERTY(ReplicatedUsing = OnRep_Level, EditAnywhere, BlueprintReadWrite, Category = "XpLevel")
	int Level;

	// 🔥 EVENTOS
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnXpChanged OnXpChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnLevelChanged OnLevelChanged;

	// 🔥 FUNCIONES
	UFUNCTION(BlueprintCallable, Category = "XpLevel")
	void IncreaseXp(float XpPoints);

	UFUNCTION(BlueprintCallable, Category = "XpLevel")
	void IncreaseLevel(int AddedLevel);

protected:
	virtual void BeginPlay() override;

	// 🔥 ONREP
	UFUNCTION()
	void OnRep_Xp();

	UFUNCTION()
	void OnRep_Level();

	UFUNCTION()
	void OnRep_MaxXp();

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};