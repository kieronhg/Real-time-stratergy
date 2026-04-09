#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/RTSFoodSource.h"
#include "RTSWildFoodResource.generated.h"

/**
 * A wild food resource (berry bush, mushroom patch, fruit tree, etc.).
 * Accessible by any team. Regenerates food slowly over time.
 * Can be depleted and will visually reflect its state via Blueprint events.
 */
UCLASS()
class REALTIMESTRATEGY_API ARTSWildFoodResource : public AActor, public IRTSFoodSource
{
	GENERATED_BODY()

public:
	ARTSWildFoodResource();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// --- IRTSFoodSource ---
	virtual bool HasFood_Implementation() const override;
	virtual float GetAvailableFood_Implementation() const override;
	virtual float TakeFood_Implementation(float RequestedAmount) override;
	virtual FVector GetFoodLocation_Implementation() const override;
	virtual int32 GetOwningTeam_Implementation() const override { return -1; } // wild = any team

	// --- Config ---

	/** Maximum food this resource can hold. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RTS|Food")
	float MaxFood = 50.f;

	/** Food regenerated per second when not fully depleted. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RTS|Food")
	float RegenerationRate = 1.f;

	/** How many citizens can eat from this resource simultaneously. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RTS|Food")
	int32 MaxConcurrentUsers = 3;

	// --- State ---

	UPROPERTY(BlueprintReadOnly, Category = "RTS|Food")
	float CurrentFood = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "RTS|Food")
	int32 CurrentUsers = 0;

	bool HasCapacityForUser() const { return CurrentUsers < MaxConcurrentUsers; }
	void RegisterUser()   { CurrentUsers = FMath::Min(CurrentUsers + 1, MaxConcurrentUsers); }
	void UnregisterUser() { CurrentUsers = FMath::Max(CurrentUsers - 1, 0); }

protected:
	/** Called in Blueprint when food level goes from non-zero to zero (show depleted visuals). */
	UFUNCTION(BlueprintImplementableEvent, Category = "RTS|Food")
	void OnDepleted();

	/** Called in Blueprint when food regenerates back above zero (restore visuals). */
	UFUNCTION(BlueprintImplementableEvent, Category = "RTS|Food")
	void OnReplenished();

private:
	bool bWasDepleted = false;
};
