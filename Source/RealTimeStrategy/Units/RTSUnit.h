#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RTSUnit.generated.h"

class UAIPerceptionStimuliSourceComponent;

UENUM(BlueprintType)
enum class EUnitState : uint8
{
	Idle       UMETA(DisplayName = "Idle"),
	Moving     UMETA(DisplayName = "Moving"),
	Attacking  UMETA(DisplayName = "Attacking"),
	Dead       UMETA(DisplayName = "Dead")
};

/**
 * Base class for all RTS units (soldiers, vehicles, etc.).
 * Handles health, team assignment, selection visuals, and AI orders.
 */
UCLASS()
class REALTIMESTRATEGY_API ARTSUnit : public ACharacter
{
	GENERATED_BODY()

public:
	ARTSUnit();

	virtual void BeginPlay() override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

	// --- Team ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RTS|Unit")
	int32 TeamId = 0;

	// --- Stats ---
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RTS|Unit")
	float MaxHealth = 100.f;

	UPROPERTY(BlueprintReadOnly, Category = "RTS|Unit")
	float CurrentHealth = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RTS|Unit")
	float AttackDamage = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RTS|Unit")
	float AttackRange = 200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RTS|Unit")
	float AttackCooldown = 1.f;

	// --- State ---
	UPROPERTY(BlueprintReadOnly, Category = "RTS|Unit")
	EUnitState UnitState = EUnitState::Idle;

	UPROPERTY(BlueprintReadOnly, Category = "RTS|Unit")
	bool bIsSelected = false;

	// --- Commands (called by player controller / AI) ---
	UFUNCTION(BlueprintCallable, Category = "RTS|Unit")
	void IssueMoveOrder(const FVector& Destination);

	UFUNCTION(BlueprintCallable, Category = "RTS|Unit")
	void IssueAttackOrder(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "RTS|Unit")
	void IssueStopOrder();

	/** Update selection highlight. */
	UFUNCTION(BlueprintCallable, Category = "RTS|Unit")
	void SetSelected(bool bSelected);

	UFUNCTION(BlueprintPure, Category = "RTS|Unit")
	bool IsAlive() const { return UnitState != EUnitState::Dead; }

protected:
	void Die();

	/** Override in Blueprint to show/hide selection decal or circle. */
	UFUNCTION(BlueprintImplementableEvent, Category = "RTS|Unit")
	void OnSelectionChanged(bool bSelected);

	/** Override in Blueprint to play death FX / animation. */
	UFUNCTION(BlueprintImplementableEvent, Category = "RTS|Unit")
	void OnDeath();

private:
	float AttackCooldownRemaining = 0.f;
};
