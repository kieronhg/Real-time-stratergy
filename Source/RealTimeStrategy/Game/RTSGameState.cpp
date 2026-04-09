#include "Game/RTSGameState.h"
#include "Game/RTSGameMode.h"
#include "Kismet/GameplayStatics.h"

ARTSGameState::ARTSGameState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ARTSGameState::BeginPlay()
{
	Super::BeginPlay();
}

void ARTSGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MatchTime += DeltaTime;
}

void ARTSGameState::RegisterTeamDefeat(int32 TeamId)
{
	if (!DefeatedTeams.Contains(TeamId))
	{
		DefeatedTeams.Add(TeamId);

		if (ARTSGameMode* GameMode = Cast<ARTSGameMode>(UGameplayStatics::GetGameMode(this)))
		{
			GameMode->OnTeamDefeated(TeamId);
		}
	}
}
