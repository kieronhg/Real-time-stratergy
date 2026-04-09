#include "Player/RTSPlayerController.h"
#include "Units/RTSUnit.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/World.h"

ARTSPlayerController::ARTSPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
	PrimaryActorTick.bCanEverTick = true;
}

void ARTSPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ARTSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("Select",         IE_Pressed, this, &ARTSPlayerController::OnSelectPressed);
	InputComponent->BindAction("Command",        IE_Pressed, this, &ARTSPlayerController::OnCommandPressed);
	InputComponent->BindAction("AddToSelection", IE_Pressed, this, &ARTSPlayerController::OnAddToSelectionPressed);

	InputComponent->BindAxis("CameraForward", this, &ARTSPlayerController::MoveCameraForward);
	InputComponent->BindAxis("CameraRight",   this, &ARTSPlayerController::MoveCameraRight);
	InputComponent->BindAxis("CameraZoom",    this, &ARTSPlayerController::ZoomCamera);
}

void ARTSPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (APawn* ControlledPawn = GetPawn())
	{
		FVector CameraLocation = ControlledPawn->GetActorLocation();

		FVector Forward = FVector(1.f, 0.f, 0.f);
		FVector Right   = FVector(0.f, 1.f, 0.f);

		CameraLocation += Forward * CameraForwardInput * CameraSpeed * DeltaTime;
		CameraLocation += Right   * CameraRightInput   * CameraSpeed * DeltaTime;

		// Zoom by adjusting height
		CameraLocation.Z = FMath::Clamp(
			CameraLocation.Z + CameraZoomInput * CameraZoomSpeed * DeltaTime,
			MinCameraHeight, MaxCameraHeight);

		ControlledPawn->SetActorLocation(CameraLocation);
	}
}

void ARTSPlayerController::OnSelectPressed()
{
	ClearSelection();

	FHitResult Hit;
	if (GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Pawn), false, Hit))
	{
		if (ARTSUnit* Unit = Cast<ARTSUnit>(Hit.GetActor()))
		{
			if (Unit->TeamId == TeamId)
			{
				SelectedUnits.Add(Unit);
				Unit->SetSelected(true);
			}
		}
	}
}

void ARTSPlayerController::OnCommandPressed()
{
	if (SelectedUnits.Num() == 0) return;

	FHitResult Hit;
	if (GetMouseHitOnGround(Hit))
	{
		IssueCommandAt(Hit.ImpactPoint);
	}
}

void ARTSPlayerController::OnAddToSelectionPressed()
{
	FHitResult Hit;
	if (GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Pawn), false, Hit))
	{
		if (ARTSUnit* Unit = Cast<ARTSUnit>(Hit.GetActor()))
		{
			if (Unit->TeamId == TeamId && !SelectedUnits.Contains(Unit))
			{
				SelectedUnits.Add(Unit);
				Unit->SetSelected(true);
			}
		}
	}
}

void ARTSPlayerController::MoveCameraForward(float Value)
{
	CameraForwardInput = Value;
}

void ARTSPlayerController::MoveCameraRight(float Value)
{
	CameraRightInput = Value;
}

void ARTSPlayerController::ZoomCamera(float Value)
{
	CameraZoomInput = Value;
}

bool ARTSPlayerController::GetMouseHitOnGround(FHitResult& OutHit) const
{
	return GetHitResultUnderCursorByChannel(
		UEngineTypes::ConvertToTraceType(ECC_WorldStatic), false, OutHit);
}

void ARTSPlayerController::ClearSelection()
{
	for (ARTSUnit* Unit : SelectedUnits)
	{
		if (IsValid(Unit))
		{
			Unit->SetSelected(false);
		}
	}
	SelectedUnits.Empty();
}

void ARTSPlayerController::IssueCommandAt(const FVector& WorldLocation)
{
	for (ARTSUnit* Unit : SelectedUnits)
	{
		if (IsValid(Unit))
		{
			Unit->IssueMoveOrder(WorldLocation);
		}
	}
}
