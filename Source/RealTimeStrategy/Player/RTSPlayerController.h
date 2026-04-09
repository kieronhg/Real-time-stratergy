#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RTSPlayerController.generated.h"

class ARTSUnit;
class URTSSelectionComponent;

/**
 * Player controller for RTS games.
 * Handles top-down camera movement, unit selection, and issuing commands.
 */
UCLASS()
class REALTIMESTRATEGY_API ARTSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ARTSPlayerController();

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaTime) override;

	/** Team this player belongs to (0-based). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RTS|Player")
	int32 TeamId = 0;

	/** Currently selected units. */
	UPROPERTY(BlueprintReadOnly, Category = "RTS|Selection")
	TArray<ARTSUnit*> SelectedUnits;

protected:
	// --- Camera ---
	UPROPERTY(EditDefaultsOnly, Category = "RTS|Camera")
	float CameraSpeed = 1500.f;

	UPROPERTY(EditDefaultsOnly, Category = "RTS|Camera")
	float CameraZoomSpeed = 500.f;

	UPROPERTY(EditDefaultsOnly, Category = "RTS|Camera")
	float MinCameraHeight = 300.f;

	UPROPERTY(EditDefaultsOnly, Category = "RTS|Camera")
	float MaxCameraHeight = 3000.f;

	float CameraForwardInput = 0.f;
	float CameraRightInput = 0.f;
	float CameraZoomInput = 0.f;

	// --- Input handlers ---
	void OnSelectPressed();
	void OnCommandPressed();
	void OnAddToSelectionPressed();

	void MoveCameraForward(float Value);
	void MoveCameraRight(float Value);
	void ZoomCamera(float Value);

	/** Perform a ground trace under the cursor and return the hit result. */
	bool GetMouseHitOnGround(FHitResult& OutHit) const;

	/** Deselect all currently selected units. */
	void ClearSelection();

	/** Issue a move/attack command to selected units at the given world location. */
	void IssueCommandAt(const FVector& WorldLocation);
};
