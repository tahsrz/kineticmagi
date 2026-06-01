#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DayNightProfile.h"
#include "DayNightSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDayPhaseChanged, EDayPhase, OldPhase, EDayPhase, NewPhase);

UCLASS()
class KINETICMAGI_API UDayNightSubsystem : public UWorldSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override;
	virtual UWorld* GetTickableGameObjectWorld() const override;

	UFUNCTION(BlueprintCallable, Category = "DayNight")
	void SetProfile(UDayNightProfile* InProfile);

	UFUNCTION(BlueprintCallable, Category = "DayNight")
	void SetPaused(bool bInPaused);

	UFUNCTION(BlueprintCallable, Category = "DayNight")
	void SetCurrentHour(float InHour);

	UFUNCTION(BlueprintPure, Category = "DayNight")
	float GetCurrentHour() const { return CurrentHour; }

	UFUNCTION(BlueprintPure, Category = "DayNight")
	float GetNormalizedDayProgress() const { return CurrentHour / 24.0f; }

	UFUNCTION(BlueprintPure, Category = "DayNight")
	EDayPhase GetCurrentPhase() const { return CurrentPhase; }

	UPROPERTY(BlueprintAssignable, Category = "DayNight")
	FOnDayPhaseChanged OnDayPhaseChanged;

private:
	EDayPhase ResolvePhase(float Hour) const;
	void AdvanceTime(float DeltaTime);

	UPROPERTY(EditAnywhere, Category = "DayNight")
	TObjectPtr<UDayNightProfile> ActiveProfile;

	float CurrentHour = 8.0f;
	bool bPaused = false;
	EDayPhase CurrentPhase = EDayPhase::Day;
};
