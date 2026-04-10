#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RTSCitizenRoleComponent.generated.h"

class ARTSCitizenAIController;

/**
 * All assignable citizen roles.
 * Add new roles here — no other file needs to change to register the enum value.
 */
UENUM(BlueprintType)
enum class ECitizenRole : uint8
{
	None        UMETA(DisplayName = "None"),
	Gatherer    UMETA(DisplayName = "Gatherer"),     // collects food from wild resources and deposits it
	Farmer      UMETA(DisplayName = "Farmer"),        // works a farm building to produce food
	Lumberjack  UMETA(DisplayName = "Lumberjack"),    // fells trees and collects wood
	Builder     UMETA(DisplayName = "Builder"),       // constructs and repairs buildings
	Guard       UMETA(DisplayName = "Guard"),         // patrols an assigned area and attacks enemies
};

/**
 * Holds the citizen's current role and drives the work behaviour for that role
 * while the citizen is fed and idle.
 *
 * The AI controller calls TickWork() from the Idle state.
 * Each role's logic lives in its own private method, keeping the controller clean.
 * Adding a new role = add to ECitizenRole + add one TickWork_<Role>() method here.
 */
UCLASS(ClassGroup=(RTS), meta=(BlueprintSpawnableComponent))
class REALTIMESTRATEGY_API URTSCitizenRoleComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URTSCitizenRoleComponent();

	// --- Role assignment ---

	/** The role currently assigned to this citizen. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RTS|Role")
	ECitizenRole CurrentRole = ECitizenRole::None;

	/**
	 * Assign a new role to this citizen.
	 * Called by the player controller when the player picks a role from the UI.
	 * Resets any in-progress work state so the citizen starts fresh.
	 */
	UFUNCTION(BlueprintCallable, Category = "RTS|Role")
	void SetRole(ECitizenRole NewRole);

	// --- Work tick ---

	/**
	 * Called each AI tick while the citizen is alive, fed, and not interrupted.
	 * Dispatches to the appropriate role work method.
	 */
	void TickWork(float DeltaTime, ARTSCitizenAIController* Controller);

	// --- Events (Blueprint-assignable) ---

	/** Fired when the citizen's role changes. */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRoleChanged, ECitizenRole, OldRole, ECitizenRole, NewRole);

	UPROPERTY(BlueprintAssignable, Category = "RTS|Role")
	FOnRoleChanged OnRoleChanged;

private:
	/** Reset any transient work state when the role changes or is cleared. */
	void ResetWorkState();

	// --- Per-role work methods ---
	// Each method is responsible for one role's full work loop.
	// Stub implementations log intent; replace with real logic as systems are built.

	void TickWork_Gatherer  (float DeltaTime, ARTSCitizenAIController* Controller);
	void TickWork_Farmer    (float DeltaTime, ARTSCitizenAIController* Controller);
	void TickWork_Lumberjack(float DeltaTime, ARTSCitizenAIController* Controller);
	void TickWork_Builder   (float DeltaTime, ARTSCitizenAIController* Controller);
	void TickWork_Guard     (float DeltaTime, ARTSCitizenAIController* Controller);
};
