#pragma once

#include "CoreMinimal.h"
#include "Units/RTSUnit.h"
#include "RTSCitizen.generated.h"

class URTSHungerComponent;

/**
 * A citizen unit owned by the player.
 * Has hunger that decays over time; the AI controller will
 * autonomously seek food from wild resources or team storage.
 */
UCLASS()
class REALTIMESTRATEGY_API ARTSCitizen : public ARTSUnit
{
	GENERATED_BODY()

public:
	ARTSCitizen();

	/** Hunger state — queried each tick by RTSCitizenAIController. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RTS|Citizen")
	TObjectPtr<URTSHungerComponent> HungerComponent;

	/** How close the citizen must be to a food source to start eating (cm). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RTS|Citizen")
	float EatingRange = 150.f;

	/** Food consumed per second while eating. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RTS|Citizen")
	float EatingRate = 10.f;

protected:
	virtual void BeginPlay() override;
};
