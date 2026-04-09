#include "Components/RTSHungerComponent.h"

URTSHungerComponent::URTSHungerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.5f; // update every half-second for performance
}

void URTSHungerComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHunger = MaxHunger;
}

void URTSHungerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CurrentHunger = FMath::Max(0.f, CurrentHunger - HungerDecayRate * DeltaTime);

	// Broadcast events exactly once per threshold crossing
	if (!bHungryEventFired && IsHungry())
	{
		bHungryEventFired = true;
		OnHungry.Broadcast();
	}
	else if (bHungryEventFired && !IsHungry())
	{
		bHungryEventFired = false; // reset so it fires again next time
	}

	if (!bStarvingEventFired && IsStarving())
	{
		bStarvingEventFired = true;
		OnStarving.Broadcast();
	}
	else if (bStarvingEventFired && !IsStarving())
	{
		bStarvingEventFired = false;
	}
}

void URTSHungerComponent::ConsumeFood(float Amount)
{
	if (Amount <= 0.f) return;

	CurrentHunger = FMath::Min(MaxHunger, CurrentHunger + Amount);
	OnFoodConsumed.Broadcast(Amount);
}
