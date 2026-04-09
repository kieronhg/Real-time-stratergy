#include "AI/RTSAIController.h"
#include "Units/RTSUnit.h"
#include "Navigation/PathFollowingComponent.h"

ARTSAIController::ARTSAIController()
{
	bWantsPlayerState = false;
}

void ARTSAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	if (ARTSUnit* Unit = GetControlledUnit())
	{
		if (Unit->UnitState == EUnitState::Moving)
		{
			Unit->UnitState = EUnitState::Idle;
		}
	}
}

ARTSUnit* ARTSAIController::GetControlledUnit() const
{
	return Cast<ARTSUnit>(GetPawn());
}
