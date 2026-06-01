#include "StealthSubsystem.h"

#include "DayNightSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UStealthSubsystem::Tick(const float DeltaTime)
{
	RefreshNightState();

	if (!bNightStealthActive)
	{
		Suspicion = FMath::Max(0.0f, Suspicion - DeltaTime * 0.35f);
		return;
	}

	const float LightPressure = PlayerLightExposure01 * 0.25f;
	const float SafeHouseModifier = bPlayerInSafeHouse ? -0.30f : 0.0f;
	Suspicion = FMath::Clamp(Suspicion + ((LightPressure + SafeHouseModifier) * DeltaTime), 0.0f, 1.0f);
}

TStatId UStealthSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UStealthSubsystem, STATGROUP_Tickables);
}

bool UStealthSubsystem::IsTickable() const
{
	return GetWorld() != nullptr;
}

UWorld* UStealthSubsystem::GetTickableGameObjectWorld() const
{
	return GetWorld();
}

void UStealthSubsystem::AddNoiseEvent(const float NoiseAmount)
{
	if (!bNightStealthActive)
	{
		return;
	}

	const float SafeHouseNoiseScale = bPlayerInSafeHouse ? 0.35f : 1.0f;
	Suspicion = FMath::Clamp(Suspicion + (NoiseAmount * SafeHouseNoiseScale), 0.0f, 1.0f);
}

void UStealthSubsystem::SetPlayerLightExposure(const float InExposure01)
{
	PlayerLightExposure01 = FMath::Clamp(InExposure01, 0.0f, 1.0f);
}

void UStealthSubsystem::SetPlayerIsInSafeHouse(const bool bInSafeHouse)
{
	bPlayerInSafeHouse = bInSafeHouse;
}

void UStealthSubsystem::RefreshNightState()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const UDayNightSubsystem* DayNight = World->GetSubsystem<UDayNightSubsystem>();
	if (!DayNight)
	{
		return;
	}

	const bool bWasNight = bNightStealthActive;
	bNightStealthActive = DayNight->GetCurrentPhase() == EDayPhase::Night;

	if (bWasNight != bNightStealthActive)
	{
		if (!bNightStealthActive)
		{
			Suspicion = 0.0f;
		}
		OnStealthNightStateChanged.Broadcast(bNightStealthActive);
	}
}
