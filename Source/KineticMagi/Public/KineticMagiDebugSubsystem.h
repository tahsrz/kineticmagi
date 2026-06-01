#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "KineticMagiDebugSubsystem.generated.h"

UCLASS()
class KINETICMAGI_API UKineticMagiDebugSubsystem : public UWorldSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override;
	virtual UWorld* GetTickableGameObjectWorld() const override;

	UFUNCTION(BlueprintCallable, Category = "Debug")
	void SetDebugOverlayEnabled(bool bEnabled);

	UFUNCTION(BlueprintPure, Category = "Debug")
	bool IsDebugOverlayEnabled() const { return bDebugOverlayEnabled; }

	UFUNCTION(BlueprintCallable, Category = "Debug")
	void SetOverlayRefreshSeconds(float Seconds);

private:
	void DrawOverlay();

	bool bDebugOverlayEnabled = true;
	float OverlayRefreshSeconds = 0.25f;
	float OverlayAccum = 0.0f;
};
