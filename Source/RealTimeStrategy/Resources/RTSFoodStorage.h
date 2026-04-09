#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/RTSFoodSource.h"
#include "RTSFoodStorage.generated.h"

/**
 * A team-owned food storage building (granary, storehouse, etc.).
 * Only accessible by citizens of the owning team.
 * Citizens or gatherers deposit food here; hungry citizens withdraw from it.
 */
UCLASS()
class REALTIMESTRATEGY_API ARTSFoodStorage : public AActor, public IRTSFoodSource
{
	GENERATED_BODY()

public:
	ARTSFoodStorage();

	// --- IRTSFoodSource ---
	virtual bool HasFood_Implementation() const override;
	virtual float GetAvailableFood_Implementation() const override;
	virtual float TakeFood_Implementation(float RequestedAmount) override;
	virtual FVector GetFoodLocation_Implementation() const override;
	virtual int32 GetOwningTeam_Implementation() const override { return TeamId; }

	// --- Config ---

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RTS|Storage")
	int32 TeamId = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RTS|Storage")
	float MaxCapacity = 500.f;

	// --- State ---

	UPROPERTY(BlueprintReadOnly, Category = "RTS|Storage")
	float StoredFood = 100.f;

	/**
	 * Deposit food into storage (e.g. from a gatherer returning with food).
	 * Returns the amount actually stored (capped by remaining capacity).
	 */
	UFUNCTION(BlueprintCallable, Category = "RTS|Storage")
	float DepositFood(float Amount);

	UFUNCTION(BlueprintPure, Category = "RTS|Storage")
	float GetFillPercent() const { return StoredFood / MaxCapacity; }

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "RTS|Storage")
	void OnStorageChanged(float NewAmount);
};
