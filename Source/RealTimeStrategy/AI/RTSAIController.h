#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "RTSAIController.generated.h"

class ARTSUnit;

/**
 * AI controller for RTS units.
 * Handles movement completion callbacks and basic idle behavior.
 */
UCLASS()
class REALTIMESTRATEGY_API ARTSAIController : public AAIController
{
	GENERATED_BODY()

public:
	ARTSAIController();

protected:
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

private:
	ARTSUnit* GetControlledUnit() const;
};
