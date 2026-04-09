#include "AI/RTSCitizenAIController.h"
#include "Units/RTSCitizen.h"
#include "Components/RTSHungerComponent.h"
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

	switch (CitizenState)
	{
	case ECitizenAIState::Idle:
		// Start seeking food as soon as we are hungry
		if (Hunger->IsHungry())
		{
			if (FindBestFoodSource())
			{
				MoveToFoodSource();
				CitizenState = ECitizenAIState::SeekingFood;
			}
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
		if (ARTSWildFoodResource* Wild = Cast<ARTSWildFoodResource>(Target))
		{
			Wild->UnregisterUser();
		}

		// Try to find another source
		if (FindBestFoodSource())
		{
			MoveToFoodSource();
			CitizenState = ECitizenAIState::SeekingFood;
		}
		else
		{
			GoIdle();
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
