#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RTSFoodSource.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class URTSFoodSource : public UInterface
{
	GENERATED_BODY()
};

/**
 * Implemented by anything a citizen can eat from:
 * wild food resources (berries, mushrooms) and food storage buildings.
 */
class REALTIMESTRATEGY_API IRTSFoodSource
{
	GENERATED_BODY()

public:
	/** Returns true if there is any food available. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RTS|Food")
	bool HasFood() const;

	/** Returns how much food is currently available. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RTS|Food")
	float GetAvailableFood() const;

	/**
	 * Attempt to take up to RequestedAmount of food.
	 * Returns the amount actually provided (may be less if supply is limited).
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RTS|Food")
	float TakeFood(float RequestedAmount);

	/**
	 * World-space location a citizen should walk to before eating.
	 * (May differ from the actor's origin for large buildings.)
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RTS|Food")
	FVector GetFoodLocation() const;

	/**
	 * Team ID that can access this food source.
	 * -1 means any team (wild resource).
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RTS|Food")
	int32 GetOwningTeam() const;
};
