#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BrokenItem.generated.h"

UCLASS()
class GAME3D_API ABrokenItem : public AActor
{
	GENERATED_BODY()
    
public:
	ABrokenItem();

	UFUNCTION(BlueprintCallable)
	void ApplyBreakImpulse(const FVector& HitDirection);

protected:
	virtual void BeginPlay() override;

	// Solo vida
	UPROPERTY(EditAnywhere, Category="Break")
	float LifeTime = 5.f;

	// Solo fuerza
	UPROPERTY(EditAnywhere, Category="Break")
	float ImpulseStrength = 1500.f;

	// Lista automática de piezas encontradas en BP
	UPROPERTY()
	TArray<UStaticMeshComponent*> Pieces;
};