#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RTSHungerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHungrySignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStarvingSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFoodConsumedSignature, float, Amount);

/**
 * Tracks a citizen's hunger level and decays it over time.
 * Broadcasts events when the citizen becomes hungry or starving.
 */
UCLASS(ClassGroup=(RTS), meta=(BlueprintSpawnableComponent))
class REALTIMESTRATEGY_API URTSHungerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URTSHungerComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	// --- Config ---

	/** Maximum hunger value (full). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RTS|Hunger")
	float MaxHunger = 100.f;

	/** Units of hunger lost per second. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RTS|Hunger")
	float HungerDecayRate = 3.f;

	/** Hunger level at which the citizen starts seeking food. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RTS|Hunger")
	float HungryThreshold = 60.f;

	/** Hunger level considered critical — citizen stops all other work. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RTS|Hunger")
	float StarvingThreshold = 25.f;

	// --- State ---

	UPROPERTY(BlueprintReadOnly, Category = "RTS|Hunger")
	float CurrentHunger = 100.f;

	// --- Queries ---

	UFUNCTION(BlueprintPure, Category = "RTS|Hunger")
	bool IsHungry() const { return CurrentHunger < HungryThreshold; }

	UFUNCTION(BlueprintPure, Category = "RTS|Hunger")
	bool IsStarving() const { return CurrentHunger < StarvingThreshold; }

	UFUNCTION(BlueprintPure, Category = "RTS|Hunger")
	float GetHungerPercent() const { return CurrentHunger / MaxHunger; }

	// --- Actions ---

	/**
	 * Restore hunger by Amount (clamped to MaxHunger).
	 * Called by the AI controller when the citizen eats.
	 */
	UFUNCTION(BlueprintCallable, Category = "RTS|Hunger")
	void ConsumeFood(float Amount);

	// --- Events ---

	/** Fired once when hunger drops below HungryThreshold. */
	UPROPERTY(BlueprintAssignable, Category = "RTS|Hunger")
	FOnHungrySignature OnHungry;

	/** Fired once when hunger drops below StarvingThreshold. */
	UPROPERTY(BlueprintAssignable, Category = "RTS|Hunger")
	FOnStarvingSignature OnStarving;

	/** Fired each time the citizen successfully eats. */
	UPROPERTY(BlueprintAssignable, Category = "RTS|Hunger")
	FOnFoodConsumedSignature OnFoodConsumed;

private:
	bool bHungryEventFired = false;
	bool bStarvingEventFired = false;
};
