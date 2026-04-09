#include "Units/RTSCitizen.h"
#include "Components/RTSHungerComponent.h"
#include "AI/RTSCitizenAIController.h"

ARTSCitizen::ARTSCitizen()
{
	HungerComponent = CreateDefaultSubobject<URTSHungerComponent>(TEXT("HungerComponent"));
	AIControllerClass = ARTSCitizenAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ARTSCitizen::BeginPlay()
{
	Super::BeginPlay();
}
