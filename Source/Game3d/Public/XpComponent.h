// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "XpComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnXpChanged, float, Xp, float, MaxXp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelChanged, int, level);
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAME3D_API UXpComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UXpComponent();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XpLevel")
	float Xp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XpLevel")
	float MaxXp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XpLevel")
	int Level;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnXpChanged OnXpChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnLevelChanged OnLevelChanged;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "XpLevel")
	void IncreaseLevel(float AddedLevel);

	UFUNCTION(BlueprintCallable, Category = "XpLevel")
	void IncreaseXp(float XpPoints);
};
