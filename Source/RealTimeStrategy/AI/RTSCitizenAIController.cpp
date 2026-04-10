#include "AI/RTSCitizenAIController.h"
#include "Units/RTSCitizen.h"
#include "Components/RTSHungerComponent.h"
#include "Components/RTSCitizenRoleComponent.h"
#include "Interfaces/RTSFoodSource.h"
#include "Resources/RTSWildFoodResource.h"
#include "Resources/RTSFoodStorage.h"
#include "Navigation/PathFollowingComponent.h"
#include "Kismet/GameplayStatics.h"

ARTSCitizenAIController::ARTSCitizenAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.25f; // evaluate AI state 4× per second
}

void ARTSCitizenAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	CitizenState = ECitizenAIState::Idle;
}

void ARTSCitizenAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ARTSCitizen* Citizen = GetCitizen();
	if (!Citizen || !Citizen->IsAlive()) return;

	URTSHungerComponent* Hunger = Citizen->HungerComponent;
	if (!Hunger) return;

	// -----------------------------------------------------------------------
	// Starvation interrupt: drop whatever we are doing and seek food immediately.
	// Skipped if we are already handling it (Starving or Eating).
	// -----------------------------------------------------------------------
	if (Hunger->IsStarving() &&
		CitizenState != ECitizenAIState::Starving &&
		CitizenState != ECitizenAIState::Eating)
	{
		AbandonCurrentFoodTarget();
		CitizenState = ECitizenAIState::Starving;
		// Fall through to the switch so the Starving case runs this same tick.
	}

	switch (CitizenState)
	{
	case ECitizenAIState::Idle:
		// Hunger takes priority over work
		if (Hunger->IsHungry())
		{
			if (FindBestFoodSource())
			{
				MoveToFoodSource();
				CitizenState = ECitizenAIState::SeekingFood;
			}
			break;
		}

		// Dispatch to role work while the citizen is fed
		if (Citizen->RoleComponent)
		{
			Citizen->RoleComponent->TickWork(DeltaTime, this);
		}
		break;

	case ECitizenAIState::SeekingFood:
	{
		// If we are no longer hungry (e.g. player fed us manually) go idle
		if (!Hunger->IsHungry())
		{
			GoIdle();
			break;
		}

		// Target became invalid or ran out of food — re-evaluate
		AActor* Target = TargetFoodActor.Get();
		if (!IsValid(Target) || !IRTSFoodSource::Execute_HasFood(Target))
		{
			if (FindBestFoodSource())
			{
				MoveToFoodSource();
			}
			else
			{
				GoIdle(); // no food anywhere right now
			}
			break;
		}

		// Arrived at the food source
		if (IsWithinEatingRange())
		{
			StopMovement();
			CitizenState = ECitizenAIState::Eating;
		}
		break;
	}

	case ECitizenAIState::Starving:
	{
		// Recovered above the starving threshold — downgrade to normal food-seeking or idle
		if (!Hunger->IsStarving())
		{
			CitizenState = Hunger->IsHungry()
				? ECitizenAIState::SeekingFood   // still hungry, keep heading to food
				: ECitizenAIState::Idle;          // fully recovered
			break;
		}

		// Target became invalid or ran out — re-search immediately (storage first, wild second)
		AActor* Target = TargetFoodActor.Get();
		if (!IsValid(Target) || !IRTSFoodSource::Execute_HasFood(Target))
		{
			AbandonCurrentFoodTarget();
			if (FindBestFoodSource())
			{
				MoveToFoodSource();
			}
			// else: no food exists anywhere; stay in Starving and retry next tick
			break;
		}

		// Arrived at the food source
		if (IsWithinEatingRange())
		{
			StopMovement();
			CitizenState = ECitizenAIState::Eating;
		}
		break;
	}

	case ECitizenAIState::Eating:
		TickEating(DeltaTime);
		break;
	}
}

void ARTSCitizenAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	// Let the Tick handle state transitions; don't call Super which sets unit to Idle
	// (we handle that ourselves for citizens)
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

ARTSCitizen* ARTSCitizenAIController::GetCitizen() const
{
	return Cast<ARTSCitizen>(GetPawn());
}

bool ARTSCitizenAIController::FindBestFoodSource()
{
	ARTSCitizen* Citizen = GetCitizen();
	if (!Citizen) return false;

	const FVector MyLocation = Citizen->GetActorLocation();
	const int32 MyTeam = Citizen->TeamId;

	AActor* BestActor = nullptr;
	float BestDistSq = FLT_MAX;

	// --- 1. Check team-owned food storages first (preferred) ---
	TArray<AActor*> Storages;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARTSFoodStorage::StaticClass(), Storages);

	for (AActor* Actor : Storages)
	{
		ARTSFoodStorage* Storage = Cast<ARTSFoodStorage>(Actor);
		if (!Storage) continue;
		if (Storage->TeamId != MyTeam) continue;
		if (!Storage->HasFood_Implementation()) continue;

		const float DistSq = FVector::DistSquared(MyLocation, Actor->GetActorLocation());
		if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			BestActor  = Actor;
		}
	}

	if (BestActor)
	{
		TargetFoodActor = BestActor;
		return true;
	}

	// --- 2. Fall back to wild food resources ---
	TArray<AActor*> WildResources;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARTSWildFoodResource::StaticClass(), WildResources);

	for (AActor* Actor : WildResources)
	{
		ARTSWildFoodResource* Wild = Cast<ARTSWildFoodResource>(Actor);
		if (!Wild) continue;
		if (!Wild->HasFood_Implementation()) continue;
		if (!Wild->HasCapacityForUser()) continue; // skip overcrowded patches

		const float DistSq = FVector::DistSquared(MyLocation, Actor->GetActorLocation());
		if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			BestActor  = Actor;
		}
	}

	TargetFoodActor = BestActor;
	return IsValid(BestActor);
}

bool ARTSCitizenAIController::IsWithinEatingRange() const
{
	ARTSCitizen* Citizen = GetCitizen();
	AActor* Target = TargetFoodActor.Get();
	if (!Citizen || !IsValid(Target)) return false;

	const FVector FoodLoc = IRTSFoodSource::Execute_GetFoodLocation(Target);
	return FVector::Dist(Citizen->GetActorLocation(), FoodLoc) <= Citizen->EatingRange;
}

void ARTSCitizenAIController::MoveToFoodSource()
{
	AActor* Target = TargetFoodActor.Get();
	if (!IsValid(Target)) return;

	const FVector FoodLoc = IRTSFoodSource::Execute_GetFoodLocation(Target);
	MoveToLocation(FoodLoc, GetCitizen()->EatingRange * 0.9f);

	// Register with wild resource so it can track occupancy
	if (ARTSWildFoodResource* Wild = Cast<ARTSWildFoodResource>(Target))
	{
		Wild->RegisterUser();
	}
}

void ARTSCitizenAIController::TickEating(float DeltaTime)
{
	ARTSCitizen* Citizen = GetCitizen();
	if (!Citizen) return;

	URTSHungerComponent* Hunger = Citizen->HungerComponent;
	AActor* Target = TargetFoodActor.Get();

	// Finished eating — we are full
	if (!Hunger->IsHungry())
	{
		if (ARTSWildFoodResource* Wild = Cast<ARTSWildFoodResource>(Target))
		{
			Wild->UnregisterUser();
		}
		GoIdle();
		return;
	}

	// Food source disappeared or ran out
	if (!IsValid(Target) || !IRTSFoodSource::Execute_HasFood(Target))
	{
		AbandonCurrentFoodTarget();

		// If still starving, re-enter the emergency state; otherwise normal seek
		const ECitizenAIState NextSeekState = Hunger->IsStarving()
			? ECitizenAIState::Starving
			: ECitizenAIState::SeekingFood;

		if (FindBestFoodSource())
		{
			MoveToFoodSource();
			CitizenState = NextSeekState;
		}
		else
		{
			// No food found — go starving/idle depending on urgency
			CitizenState = Hunger->IsStarving() ? ECitizenAIState::Starving : ECitizenAIState::Idle;
			StopMovement();
		}
		return;
	}

	// Actually eat: pull food from the source, push it into the hunger component
	const float Wanted  = Citizen->EatingRate * DeltaTime;
	const float Received = IRTSFoodSource::Execute_TakeFood(Target, Wanted);
	if (Received > 0.f)
	{
		Hunger->ConsumeFood(Received);
	}
}

void ARTSCitizenAIController::GoIdle()
{
	StopMovement();
	TargetFoodActor.Reset();
	CitizenState = ECitizenAIState::Idle;

	if (ARTSCitizen* Citizen = GetCitizen())
	{
		Citizen->UnitState = EUnitState::Idle;
	}
}

void ARTSCitizenAIController::AbandonCurrentFoodTarget()
{
	// Unregister from any wild resource we were occupying a slot on
	if (ARTSWildFoodResource* Wild = Cast<ARTSWildFoodResource>(TargetFoodActor.Get()))
	{
		Wild->UnregisterUser();
	}
	StopMovement();
	TargetFoodActor.Reset();
}
