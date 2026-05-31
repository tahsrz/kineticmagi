// Copyright (c) TahWorld. All rights reserved.

#include "LBP_PhysicsSpringMode.h"

#include "MoverTypes.h"
#include "MoveLibrary/MovementUtilsTypes.h"

const FName ULBP_PhysicsSpringMode::ModeName(TEXT("LBP_PhysicsSpringMode"));

ULBP_PhysicsSpringMode::ULBP_PhysicsSpringMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MaxPlanarSpeed = 850.0f;
	SpringAcceleration = 4200.0f;
	SteeringDamping = 5.0f;
	ReleaseDamping = 9.0f;
	MomentumRetention = 0.82f;
	HardPlanarSpeedLimit = 1500.0f;

	// Preserve ChaosMover async simulation support from the parent mode.
	bSupportsAsync = true;
}

void ULBP_PhysicsSpringMode::GenerateMove_Implementation(
	const FMoverTickStartData& StartState,
	const FMoverTimeStep& TimeStep,
	FProposedMove& OutProposedMove) const
{
	Super::GenerateMove_Implementation(StartState, TimeStep, OutProposedMove);

	const FMoverDefaultSyncState* StartingSyncState = StartState.SyncState.SyncStateCollection.FindDataByType<FMoverDefaultSyncState>();
	if (!StartingSyncState)
	{
		return;
	}

	const float DeltaSeconds = FMath::Max(TimeStep.StepMs * 0.001f, UE_SMALL_NUMBER);
	FVector DesiredVelocity = OutProposedMove.LinearVelocity;

	if (OutProposedMove.bHasDirIntent)
	{
		FVector PlanarIntent = OutProposedMove.DirectionIntent;
		PlanarIntent.Z = 0.0f;

		const float IntentStrength = FMath::Clamp(PlanarIntent.Size(), 0.0f, 1.0f);
		const FVector IntentDirection = PlanarIntent.GetSafeNormal();
		DesiredVelocity.X = IntentDirection.X * MaxPlanarSpeed * IntentStrength;
		DesiredVelocity.Y = IntentDirection.Y * MaxPlanarSpeed * IntentStrength;
	}
	else
	{
		DesiredVelocity.X = 0.0f;
		DesiredVelocity.Y = 0.0f;
	}

	OutProposedMove.LinearVelocity = ComputeSpringVelocity(
		StartingSyncState->GetVelocity_WorldSpace(),
		DesiredVelocity,
		OutProposedMove.bHasDirIntent,
		DeltaSeconds);
}

FVector ULBP_PhysicsSpringMode::ComputeSpringVelocity(
	const FVector& CurrentVelocity,
	const FVector& DesiredVelocity,
	bool bHasDirectionalIntent,
	float DeltaSeconds) const
{
	const FVector CurrentPlanar(CurrentVelocity.X, CurrentVelocity.Y, 0.0f);
	const FVector DesiredPlanar(DesiredVelocity.X, DesiredVelocity.Y, 0.0f);

	const FVector VelocityError = DesiredPlanar - CurrentPlanar;
	const float Damping = bHasDirectionalIntent ? SteeringDamping : ReleaseDamping;
	const FVector SpringAccelerationVector = (VelocityError * SpringAcceleration) - (CurrentPlanar * Damping);
	FVector NewPlanarVelocity = CurrentPlanar + SpringAccelerationVector * DeltaSeconds;

	if (bHasDirectionalIntent && MomentumRetention > 0.0f)
	{
		const float CurrentSpeed = CurrentPlanar.Size();
		const float MinimumRetainedSpeed = CurrentSpeed * MomentumRetention;
		if (CurrentSpeed > UE_KINDA_SMALL_NUMBER && NewPlanarVelocity.Size() < MinimumRetainedSpeed)
		{
			NewPlanarVelocity = NewPlanarVelocity.GetSafeNormal(UE_SMALL_NUMBER, CurrentPlanar.GetSafeNormal()) * MinimumRetainedSpeed;
		}
	}

	if (HardPlanarSpeedLimit > 0.0f)
	{
		NewPlanarVelocity = NewPlanarVelocity.GetClampedToMaxSize(HardPlanarSpeedLimit);
	}

	return FVector(NewPlanarVelocity.X, NewPlanarVelocity.Y, DesiredVelocity.Z);
}

