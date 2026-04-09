#include "Resources/RTSWildFoodResource.h"

ARTSWildFoodResource::ARTSWildFoodResource()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.f; // regenerate once per second
}

void ARTSWildFoodResource::BeginPlay()
{
	Super::BeginPlay();
	CurrentFood = MaxFood;
}

void ARTSWildFoodResource::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentFood < MaxFood)
	{
		const bool bWasEmpty = (CurrentFood <= 0.f);
		CurrentFood = FMath::Min(MaxFood, CurrentFood + RegenerationRate * DeltaTime);

		if (bWasEmpty && CurrentFood > 0.f)
		{
			bWasDepleted = false;
			OnReplenished();
		}
	}
}

bool ARTSWildFoodResource::HasFood_Implementation() const
{
	return CurrentFood > 0.f;
}

float ARTSWildFoodResource::GetAvailableFood_Implementation() const
{
	return CurrentFood;
}

float ARTSWildFoodResource::TakeFood_Implementation(float RequestedAmount)
{
	const float Taken = FMath::Min(RequestedAmount, CurrentFood);
	CurrentFood -= Taken;

	if (CurrentFood <= 0.f && !bWasDepleted)
	{
		bWasDepleted = true;
		OnDepleted();
	}

	return Taken;
}

FVector ARTSWildFoodResource::GetFoodLocation_Implementation() const
{
	return GetActorLocation();
}
