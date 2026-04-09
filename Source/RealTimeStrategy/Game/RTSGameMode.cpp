#include "Game/RTSGameMode.h"
#include "Game/RTSGameState.h"
#include "Player/RTSPlayerController.h"
#include "Kismet/GameplayStatics.h"

ARTSGameMode::ARTSGameMode()
{
	PlayerControllerClass = ARTSPlayerController::StaticClass();
	GameStateClass = ARTSGameState::StaticClass();
}

void ARTSGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void ARTSGameMode::OnTeamDefeated(int32 TeamId)
{
	UE_LOG(LogTemp, Warning, TEXT("Team %d has been defeated!"), TeamId);
	CheckVictoryConditions();
}

void ARTSGameMode::CheckVictoryConditions()
{
	// Subclass or Blueprint can override victory logic
}
