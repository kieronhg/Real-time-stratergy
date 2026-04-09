#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "RTSGameState.generated.h"

/**
 * Tracks global match state: elapsed time, team resources, defeated teams.
 */
UCLASS()
class REALTIMESTRATEGY_API ARTSGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ARTSGameState();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	/** Elapsed match time in seconds. */
	UPROPERTY(BlueprintReadOnly, Category = "RTS|GameState")
	float MatchTime = 0.f;

	/** Teams that have been eliminated. */
	UPROPERTY(BlueprintReadOnly, Category = "RTS|GameState")
	TArray<int32> DefeatedTeams;

	/** Register a team as defeated. */
	void RegisterTeamDefeat(int32 TeamId);
};
