#include "KineticMagiDebugBPLibrary.h"

#include "AutonomousPopulationSubsystem.h"
#include "DayNightSubsystem.h"
#include "Engine/World.h"
#include "KineticMagiDebugSubsystem.h"

void UKineticMagiDebugBPLibrary::KM_SetTimeNight(UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return;
	}

	if (UWorld* World = WorldContextObject->GetWorld())
	{
		if (UDayNightSubsystem* DayNight = World->GetSubsystem<UDayNightSubsystem>())
		{
			DayNight->SetCurrentHour(22.0f);
		}
	}
}

void UKineticMagiDebugBPLibrary::KM_SetTimeDay(UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return;
	}

	if (UWorld* World = WorldContextObject->GetWorld())
	{
		if (UDayNightSubsystem* DayNight = World->GetSubsystem<UDayNightSubsystem>())
		{
			DayNight->SetCurrentHour(10.0f);
		}
	}
}

void UKineticMagiDebugBPLibrary::KM_SpawnCloneWave(UObject* WorldContextObject, const int32 Count, const float Radius)
{
	if (!WorldContextObject)
	{
		return;
	}

	if (UWorld* World = WorldContextObject->GetWorld())
	{
		if (UAutonomousPopulationSubsystem* Population = World->GetSubsystem<UAutonomousPopulationSubsystem>())
		{
			Population->SpawnInitialPopulation(Count, Radius);
		}
	}
}

void UKineticMagiDebugBPLibrary::KM_SetDebugOverlayEnabled(UObject* WorldContextObject, const bool bEnabled)
{
	if (!WorldContextObject)
	{
		return;
	}

	if (UWorld* World = WorldContextObject->GetWorld())
	{
		if (UKineticMagiDebugSubsystem* DebugSubsystem = World->GetSubsystem<UKineticMagiDebugSubsystem>())
		{
			DebugSubsystem->SetDebugOverlayEnabled(bEnabled);
		}
	}
}
