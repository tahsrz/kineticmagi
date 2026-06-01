#include "NightEnemyDirectorSubsystem.h"

#include "DayNightSubsystem.h"
#include "StealthSubsystem.h"

void UNightEnemyDirectorSubsystem::Tick(const float DeltaTime)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const UDayNightSubsystem* DayNight = World->GetSubsystem<UDayNightSubsystem>();
	const UStealthSubsystem* Stealth = World->GetSubsystem<UStealthSubsystem>();
	if (!DayNight || !Stealth)
	{
		return;
	}

	bNightAssaultActive = DayNight->GetCurrentPhase() == EDayPhase::Night;
	if (!bNightAssaultActive)
	{
		SpawnPressure = FMath::Max(0.0f, SpawnPressure - (DeltaTime * 0.6f));
		if (CurrentThreatLevel != ENightThreatLevel::Calm)
		{
			CurrentThreatLevel = ENightThreatLevel::Calm;
			OnThreatLevelChanged.Broadcast(CurrentThreatLevel);
		}
		return;
	}

	const float Suspicion = Stealth->GetSuspicion();
	const ENightThreatLevel NewThreat = ResolveThreatLevel(Suspicion);
	if (NewThreat != CurrentThreatLevel)
	{
		CurrentThreatLevel = NewThreat;
		OnThreatLevelChanged.Broadcast(CurrentThreatLevel);
	}

	const float TargetPressure = Suspicion;
	SpawnPressure = FMath::FInterpTo(SpawnPressure, TargetPressure, DeltaTime, 1.25f);
}

TStatId UNightEnemyDirectorSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UNightEnemyDirectorSubsystem, STATGROUP_Tickables);
}

bool UNightEnemyDirectorSubsystem::IsTickable() const
{
	return GetWorld() != nullptr;
}

UWorld* UNightEnemyDirectorSubsystem::GetTickableGameObjectWorld() const
{
	return GetWorld();
}

ENightThreatLevel UNightEnemyDirectorSubsystem::ResolveThreatLevel(const float Suspicion) const
{
	if (Suspicion < 0.2f)
	{
		return ENightThreatLevel::Calm;
	}
	if (Suspicion < 0.5f)
	{
		return ENightThreatLevel::Hunting;
	}
	if (Suspicion < 0.8f)
	{
		return ENightThreatLevel::Alerted;
	}
	return ENightThreatLevel::Overrun;
}
