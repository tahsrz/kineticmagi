#include "KineticMagiDebugSubsystem.h"

#include "AutonomousPopulationSubsystem.h"
#include "DayNightSubsystem.h"
#include "Engine/Engine.h"
#include "StealthSubsystem.h"

void UKineticMagiDebugSubsystem::Tick(const float DeltaTime)
{
	if (!bDebugOverlayEnabled)
	{
		return;
	}

	OverlayAccum += DeltaTime;
	if (OverlayAccum < OverlayRefreshSeconds)
	{
		return;
	}

	OverlayAccum = 0.0f;
	DrawOverlay();
}

TStatId UKineticMagiDebugSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UKineticMagiDebugSubsystem, STATGROUP_Tickables);
}

bool UKineticMagiDebugSubsystem::IsTickable() const
{
	return GetWorld() != nullptr;
}

UWorld* UKineticMagiDebugSubsystem::GetTickableGameObjectWorld() const
{
	return GetWorld();
}

void UKineticMagiDebugSubsystem::SetDebugOverlayEnabled(const bool bEnabled)
{
	bDebugOverlayEnabled = bEnabled;
}

void UKineticMagiDebugSubsystem::SetOverlayRefreshSeconds(const float Seconds)
{
	OverlayRefreshSeconds = FMath::Clamp(Seconds, 0.05f, 2.0f);
}

void UKineticMagiDebugSubsystem::DrawOverlay()
{
	if (!GEngine || !GetWorld())
	{
		return;
	}

	const UDayNightSubsystem* DayNight = GetWorld()->GetSubsystem<UDayNightSubsystem>();
	const UStealthSubsystem* Stealth = GetWorld()->GetSubsystem<UStealthSubsystem>();
	const UAutonomousPopulationSubsystem* Population = GetWorld()->GetSubsystem<UAutonomousPopulationSubsystem>();

	const FString PhaseText = DayNight
		? StaticEnum<EDayPhase>()->GetValueAsString(DayNight->GetCurrentPhase())
		: TEXT("NoDayNight");

	const float Hour = DayNight ? DayNight->GetCurrentHour() : -1.0f;
	const float Suspicion = Stealth ? Stealth->GetSuspicion() : -1.0f;
	const bool bNightStealth = Stealth ? Stealth->IsNightStealthActive() : false;
	const int32 AliveClones = Population ? Population->GetAliveCloneCount() : 0;
	const int32 Raiders = Population ? Population->GetNightRaiderCount() : 0;
	const int32 UnshelteredRaiders = Population ? Population->GetUnshelteredRaiderCount() : 0;

	const FString Line = FString::Printf(
		TEXT("[KM DEBUG] Time: %.2f | Phase: %s | NightStealth: %s | Suspicion: %.2f | Clones: %d | Raiders: %d | RaidersUnshaded: %d"),
		Hour,
		*PhaseText,
		bNightStealth ? TEXT("ON") : TEXT("OFF"),
		Suspicion,
		AliveClones,
		Raiders,
		UnshelteredRaiders
	);

	GEngine->AddOnScreenDebugMessage(1001, OverlayRefreshSeconds + 0.05f, FColor::Green, Line);
}
