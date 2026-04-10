#include "Units/RTSCitizen.h"
#include "Components/RTSHungerComponent.h"
#include "Components/RTSCitizenRoleComponent.h"
#include "AI/RTSCitizenAIController.h"

ARTSCitizen::ARTSCitizen()
{
	HungerComponent = CreateDefaultSubobject<URTSHungerComponent>(TEXT("HungerComponent"));
	RoleComponent   = CreateDefaultSubobject<URTSCitizenRoleComponent>(TEXT("RoleComponent"));
	AIControllerClass = ARTSCitizenAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ARTSCitizen::BeginPlay()
{
	Super::BeginPlay();
}
