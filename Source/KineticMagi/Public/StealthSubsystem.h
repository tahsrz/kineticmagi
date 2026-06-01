#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DayNightProfile.h"
#include "StealthSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStealthNightStateChanged, bool, bNightStealthActive);

UCLASS()
class KINETICMAGI_API UStealthSubsystem : public UWorldSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override;
	virtual UWorld* GetTickableGameObjectWorld() const override;

	UFUNCTION(BlueprintCallable, Category = "Stealth")
	void AddNoiseEvent(float NoiseAmount);

	UFUNCTION(BlueprintCallable, Category = "Stealth")
	void SetPlayerLightExposure(float InExposure01);

	UFUNCTION(BlueprintCallable, Category = "Stealth")
	void SetPlayerIsInSafeHouse(bool bInSafeHouse);

	UFUNCTION(BlueprintPure, Category = "Stealth")
	float GetSuspicion() const { return Suspicion; }

	UFUNCTION(BlueprintPure, Category = "Stealth")
	float GetPlayerLightExposure() const { return PlayerLightExposure01; }

	UFUNCTION(BlueprintPure, Category = "Stealth")
	bool IsNightStealthActive() const { return bNightStealthActive; }

	UPROPERTY(BlueprintAssignable, Category = "Stealth")
	FOnStealthNightStateChanged OnStealthNightStateChanged;

private:
	void RefreshNightState();

	float Suspicion = 0.0f;
	float PlayerLightExposure01 = 0.0f;
	bool bPlayerInSafeHouse = false;
	bool bNightStealthActive = false;
};
