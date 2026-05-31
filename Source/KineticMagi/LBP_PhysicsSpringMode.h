// Copyright (c) TahWorld. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ChaosMover/Character/Modes/ChaosWalkingMode.h"
#include "LBP_PhysicsSpringMode.generated.h"

/**
 * LBP_PhysicsSpringMode
 *
 * Chaos/Mover custom movement mode for weighted platformer motion. It does not
 * directly translate the capsule. Instead, it turns player intent into a
 * spring-damped target velocity and lets ChaosMover resolve that target through
 * the physics-backed character constraint path.
 */
UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class KINETICMAGI_API ULBP_PhysicsSpringMode : public UChaosWalkingMode
{
	GENERATED_BODY()

public:
	static const FName ModeName;

	ULBP_PhysicsSpringMode(const FObjectInitializer& ObjectInitializer);

	virtual void GenerateMove_Implementation(
		const FMoverTickStartData& StartState,
		const FMoverTimeStep& TimeStep,
		FProposedMove& OutProposedMove) const override;

protected:
	/** Maximum horizontal speed reached at full directional intent. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBP Physics Spring", meta = (ClampMin = "0.0", ForceUnits = "cm/s"))
	float MaxPlanarSpeed;

	/** Spring acceleration used to pull current velocity toward desired velocity. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBP Physics Spring", meta = (ClampMin = "0.0", ForceUnits = "cm/s^2"))
	float SpringAcceleration;

	/** Damping applied while steering. Higher values make direction changes heavier. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBP Physics Spring", meta = (ClampMin = "0.0"))
	float SteeringDamping;

	/** Damping applied when directional input is released. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBP Physics Spring", meta = (ClampMin = "0.0"))
	float ReleaseDamping;

	/** Minimum retained speed ratio during direction changes. Lower values feel heavier. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBP Physics Spring", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MomentumRetention;

	/** Clamp runaway planar velocity after spring integration. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LBP Physics Spring", meta = (ClampMin = "0.0", ForceUnits = "cm/s"))
	float HardPlanarSpeedLimit;

private:
	FVector ComputeSpringVelocity(
		const FVector& CurrentVelocity,
		const FVector& DesiredVelocity,
		bool bHasDirectionalIntent,
		float DeltaSeconds) const;
};
