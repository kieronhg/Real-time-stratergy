#include "Resources/RTSFoodStorage.h"

ARTSFoodStorage::ARTSFoodStorage()
{
	PrimaryActorTick.bCanEverTick = false;
}

bool ARTSFoodStorage::HasFood_Implementation() const
{
	return StoredFood > 0.f;
}

float ARTSFoodStorage::GetAvailableFood_Implementation() const
{
	return StoredFood;
}

float ARTSFoodStorage::TakeFood_Implementation(float RequestedAmount)
{
	const float Taken = FMath::Min(RequestedAmount, StoredFood);
	StoredFood -= Taken;
	OnStorageChanged(StoredFood);
	return Taken;
}

FVector ARTSFoodStorage::GetFoodLocation_Implementation() const
{
	return GetActorLocation();
}

float ARTSFoodStorage::DepositFood(float Amount)
{
	const float Deposited = FMath::Min(Amount, MaxCapacity - StoredFood);
	StoredFood += Deposited;
	OnStorageChanged(StoredFood);
	return Deposited;
}
