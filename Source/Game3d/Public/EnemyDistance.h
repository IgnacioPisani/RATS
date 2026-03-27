#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "EnemyDistance.generated.h"

UCLASS()
class GAME3D_API AEnemyDistance : public AEnemyBase
{
	GENERATED_BODY()

public:
	AEnemyDistance();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void HandleHit_Implementation() override;
protected:

	UPROPERTY(EditAnywhere, Category="Attack")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, Category="Attack")
	float AttackCooldown = 2.0f;

private:
	float LastAttackTime;
};