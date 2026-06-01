#include "DayNightSubsystem.h"

void UDayNightSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (ActiveProfile)
	{
		CurrentHour = ActiveProfile->InitialHour;
	}

	CurrentPhase = ResolvePhase(CurrentHour);
}

void UDayNightSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UDayNightSubsystem::Tick(float DeltaTime)
{
	if (bPaused)
	{
		return;
	}

	AdvanceTime(DeltaTime);

	const EDayPhase NewPhase = ResolvePhase(CurrentHour);
	if (NewPhase != CurrentPhase)
	{
		const EDayPhase OldPhase = CurrentPhase;
		CurrentPhase = NewPhase;
		OnDayPhaseChanged.Broadcast(OldPhase, NewPhase);
	}
}

TStatId UDayNightSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UDayNightSubsystem, STATGROUP_Tickables);
}

bool UDayNightSubsystem::IsTickable() const
{
	return GetWorld() != nullptr;
}

UWorld* UDayNightSubsystem::GetTickableGameObjectWorld() const
{
	return GetWorld();
}

void UDayNightSubsystem::SetProfile(UDayNightProfile* InProfile)
{
	ActiveProfile = InProfile;
	if (ActiveProfile)
	{
		CurrentHour = ActiveProfile->InitialHour;
		CurrentPhase = ResolvePhase(CurrentHour);
	}
}

void UDayNightSubsystem::SetPaused(bool bInPaused)
{
	bPaused = bInPaused;
}

void UDayNightSubsystem::SetCurrentHour(float InHour)
{
	CurrentHour = FMath::Fmod(FMath::Max(0.0f, InHour), 24.0f);
	CurrentPhase = ResolvePhase(CurrentHour);
}

EDayPhase UDayNightSubsystem::ResolvePhase(const float Hour) const
{
	const float DawnStart = ActiveProfile ? ActiveProfile->DawnStartHour : 5.5f;
	const float DayStart = ActiveProfile ? ActiveProfile->DayStartHour : 7.0f;
	const float DuskStart = ActiveProfile ? ActiveProfile->DuskStartHour : 18.0f;
	const float NightStart = ActiveProfile ? ActiveProfile->NightStartHour : 20.0f;

	if (Hour >= DawnStart && Hour < DayStart)
	{
		return EDayPhase::Dawn;
	}
	if (Hour >= DayStart && Hour < DuskStart)
	{
		return EDayPhase::Day;
	}
	if (Hour >= DuskStart && Hour < NightStart)
	{
		return EDayPhase::Dusk;
	}
	return EDayPhase::Night;
}

void UDayNightSubsystem::AdvanceTime(const float DeltaTime)
{
	const float DayLengthMinutes = ActiveProfile ? FMath::Max(1.0f, ActiveProfile->DayLengthMinutes) : 30.0f;
	const float GameHoursPerSecond = 24.0f / (DayLengthMinutes * 60.0f);
	CurrentHour += DeltaTime * GameHoursPerSecond;

	while (CurrentHour >= 24.0f)
	{
		CurrentHour -= 24.0f;
	}
	while (CurrentHour < 0.0f)
	{
		CurrentHour += 24.0f;
	}
}
