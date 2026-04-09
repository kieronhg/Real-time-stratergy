#include "Units/RTSUnit.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ARTSUnit::ARTSUnit()
{
	PrimaryActorTick.bCanEverTick = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	bUseControllerRotationYaw = false;
}

void ARTSUnit::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
}

float ARTSUnit::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	if (!IsAlive()) return 0.f;

	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	CurrentHealth = FMath::Clamp(CurrentHealth - ActualDamage, 0.f, MaxHealth);

	if (CurrentHealth <= 0.f)
	{
		Die();
	}

	return ActualDamage;
}

void ARTSUnit::IssueMoveOrder(const FVector& Destination)
{
	if (!IsAlive()) return;

	UnitState = EUnitState::Moving;

	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		AIC->MoveToLocation(Destination, 10.f);
	}
}

void ARTSUnit::IssueAttackOrder(AActor* Target)
{
	if (!IsAlive() || !IsValid(Target)) return;

	UnitState = EUnitState::Attacking;

	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		AIC->MoveToActor(Target, AttackRange * 0.9f);
	}
}

void ARTSUnit::IssueStopOrder()
{
	UnitState = EUnitState::Idle;

	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		AIC->StopMovement();
	}
}

void ARTSUnit::SetSelected(bool bSelected)
{
	bIsSelected = bSelected;
	OnSelectionChanged(bSelected);
}

void ARTSUnit::Die()
{
	UnitState = EUnitState::Dead;
	SetSelected(false);

	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		AIC->StopMovement();
	}

	OnDeath();

	// Destroy after a short delay to allow death animation to play
	SetLifeSpan(3.f);
}
