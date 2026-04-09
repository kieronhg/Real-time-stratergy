#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RTSGameMode.generated.h"

/**
 * Game mode for the real-time strategy game.
 * Manages win/loss conditions and team setup.
 */
UCLASS()
class REALTIMESTRATEGY_API ARTSGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARTSGameMode();

	virtual void BeginPlay() override;

	/** Called when a team has no units or buildings remaining. */
	void OnTeamDefeated(int32 TeamId);

	/** Returns the number of teams in this match. */
	UFUNCTION(BlueprintCallable, Category = "RTS|GameMode")
	int32 GetNumTeams() const { return NumTeams; }

protected:
	/** Number of teams in this match. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RTS|GameMode")
	int32 NumTeams = 2;

	/** Check whether only one team remains and trigger victory. */
	void CheckVictoryConditions();
};
