#include "Components/RTSCitizenRoleComponent.h"
#include "AI/RTSCitizenAIController.h"

URTSCitizenRoleComponent::URTSCitizenRoleComponent()
{
	// Role logic is driven by the AI controller tick — no independent tick needed.
	PrimaryComponentTick.bCanEverTick = false;
}

void URTSCitizenRoleComponent::SetRole(ECitizenRole NewRole)
{
	if (NewRole == CurrentRole) return;

	const ECitizenRole OldRole = CurrentRole;
	CurrentRole = NewRole;

	ResetWorkState();
	OnRoleChanged.Broadcast(OldRole, NewRole);
}

void URTSCitizenRoleComponent::TickWork(float DeltaTime, ARTSCitizenAIController* Controller)
{
	switch (CurrentRole)
	{
	case ECitizenRole::Gatherer:    TickWork_Gatherer  (DeltaTime, Controller); break;
	case ECitizenRole::Farmer:      TickWork_Farmer    (DeltaTime, Controller); break;
	case ECitizenRole::Lumberjack:  TickWork_Lumberjack(DeltaTime, Controller); break;
	case ECitizenRole::Builder:     TickWork_Builder   (DeltaTime, Controller); break;
	case ECitizenRole::Guard:       TickWork_Guard     (DeltaTime, Controller); break;
	case ECitizenRole::None:
	default:
		break; // No role assigned — citizen sits idle.
	}
}

void URTSCitizenRoleComponent::ResetWorkState()
{
	// When roles have their own state (carry weight, target actor, patrol point, etc.)
	// reset it here so the citizen starts the new role cleanly.
}

// ---------------------------------------------------------------------------
// Per-role work stubs
// Replace each stub body with real logic as the relevant systems are built.
// ---------------------------------------------------------------------------

void URTSCitizenRoleComponent::TickWork_Gatherer(float DeltaTime, ARTSCitizenAIController* Controller)
{
	// TODO: Find nearest wild food resource or farm.
	//       Walk to it, gather food, then walk to the nearest team food storage and deposit.
	//       Repeat until role changes or citizen becomes hungry.
}

void URTSCitizenRoleComponent::TickWork_Farmer(float DeltaTime, ARTSCitizenAIController* Controller)
{
	// TODO: Find this citizen's assigned farm building.
	//       Walk to it and begin the farming work action.
	//       Produce food into the farm's output over time.
}

void URTSCitizenRoleComponent::TickWork_Lumberjack(float DeltaTime, ARTSCitizenAIController* Controller)
{
	// TODO: Find the nearest tree / lumber resource.
	//       Walk to it, chop, then deposit wood at the nearest storage.
	//       Repeat until role changes or citizen becomes hungry.
}

void URTSCitizenRoleComponent::TickWork_Builder(float DeltaTime, ARTSCitizenAIController* Controller)
{
	// TODO: Find the nearest construction site or damaged building needing repair.
	//       Walk to it and perform the build/repair action until complete.
	//       Then find the next site.
}

void URTSCitizenRoleComponent::TickWork_Guard(float DeltaTime, ARTSCitizenAIController* Controller)
{
	// TODO: Move to the assigned patrol point or guard position.
	//       Scan for enemies within detection range.
	//       Attack the nearest enemy; return to post when threat is gone.
}
