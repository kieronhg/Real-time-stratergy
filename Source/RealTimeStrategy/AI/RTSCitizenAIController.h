#pragma once

#include "CoreMinimal.h"
#include "AI/RTSAIController.h"
#include "RTSCitizenAIController.generated.h"

class ARTSCitizen;
class IRTSFoodSource;

UENUM(BlueprintType)
enum class ECitizenAIState : uint8
{
	Idle          UMETA(DisplayName = "Idle"),
	SeekingFood   UMETA(DisplayName = "Seeking Food"),   // walking toward a food source
	Eating        UMETA(DisplayName = "Eating"),          // standing still, consuming food
};

/**
 * AI controller for citizen units.
 *
 * Priority logic each tick:
 *   1. If starving  -> immediately drop everything and find food
 *   2. If hungry    -> finish current action then find food
 *   3. Otherwise    -> idle (future: work tasks)
 *
 * Food search order:
 *   1. Nearest team-owned food storage that has stock
 *   2. Nearest accessible wild food resource that has stock
 */
UCLASS()
class REALTIMESTRATEGY_API ARTSCitizenAIController : public ARTSAIController
{
	GENERATED_BODY()

public:
	ARTSCitizenAIController();

	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	UPROPERTY(BlueprintReadOnly, Category = "RTS|AI")
	ECitizenAIState CitizenState = ECitizenAIState::Idle;

private:
	TWeakObjectPtr<AActor> TargetFoodActor;   // the actor we're heading to / eating from

	ARTSCitizen* GetCitizen() const;

	/** Find the best food source reachable by this citizen and store it in TargetFoodActor. */
	bool FindBestFoodSource();

	/** Returns true if the citizen is within eating range of TargetFoodActor. */
	bool IsWithinEatingRange() const;

	/** Move toward TargetFoodActor. */
	void MoveToFoodSource();

	/** Tick eating: pull food from source, feed hunger component. */
	void TickEating(float DeltaTime);

	/** Abandon the current food target and go idle. */
	void GoIdle();
};
