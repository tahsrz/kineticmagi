#include "AutonomousPopulationSubsystem.h"

#include "DayNightSubsystem.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

void UAutonomousPopulationSubsystem::Tick(const float DeltaTime)
{
	DecisionAccumulator += DeltaTime;
	if (DecisionAccumulator < DecisionIntervalSeconds)
	{
		return;
	}

	DecisionAccumulator = 0.0f;
	CleanupDeadPawns();
	ResolveNightRaiders();
	MakeDecisions();
	ResolveCombat();
	CleanupDeadPawns();
}

TStatId UAutonomousPopulationSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UAutonomousPopulationSubsystem, STATGROUP_Tickables);
}

bool UAutonomousPopulationSubsystem::IsTickable() const
{
	return GetWorld() != nullptr;
}

UWorld* UAutonomousPopulationSubsystem::GetTickableGameObjectWorld() const
{
	return GetWorld();
}

void UAutonomousPopulationSubsystem::InitializeCloneClassFromPlayer()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const APlayerController* PC = World->GetFirstPlayerController();
	const APawn* PlayerPawn = PC ? PC->GetPawn() : nullptr;
	if (PlayerPawn)
	{
		ClonePawnClass = PlayerPawn->GetClass();
	}
}

void UAutonomousPopulationSubsystem::SpawnInitialPopulation(const int32 Count, const float Radius)
{
	if (Count <= 0)
	{
		return;
	}

	if (!ClonePawnClass)
	{
		InitializeCloneClassFromPlayer();
	}

	UWorld* World = GetWorld();
	if (!World || !ClonePawnClass)
	{
		return;
	}

	const APlayerController* PC = World->GetFirstPlayerController();
	const APawn* PlayerPawn = PC ? PC->GetPawn() : nullptr;
	const FVector Center = PlayerPawn ? PlayerPawn->GetActorLocation() : FVector::ZeroVector;

	for (int32 i = 0; i < Count; ++i)
	{
		const FVector Offset = FMath::VRand() * FMath::FRandRange(Radius * 0.35f, Radius);
		SpawnCloneAtLocation(Center + FVector(Offset.X, Offset.Y, 100.0f));
	}
}

APawn* UAutonomousPopulationSubsystem::SpawnCloneAtLocation(const FVector& Location)
{
	UWorld* World = GetWorld();
	if (!World || !ClonePawnClass)
	{
		return nullptr;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	APawn* SpawnedPawn = World->SpawnActor<APawn>(ClonePawnClass, Location, FRotator::ZeroRotator, Params);
	if (!SpawnedPawn)
	{
		return nullptr;
	}

	FCloneMindState NewState;
	NewState.Pawn = SpawnedPawn;
	NewState.Desires.Aggression = FMath::FRandRange(0.15f, 0.85f);
	NewState.Desires.DiscoveryDrive = FMath::FRandRange(0.2f, 0.95f);
	NewState.Desires.Paranoia = FMath::FRandRange(0.1f, 0.8f);
	NewState.Desires.Mercy = FMath::FRandRange(0.0f, 0.75f);
	NewState.Desires.CombatPower = FMath::FRandRange(0.75f, 1.4f);
	NewState.Intent = ECloneIntent::Idle;
	NewState.HealthNormalized = 1.0f;
	NewState.bNightRaider = false;

	CloneStates.Add(NewState);
	return SpawnedPawn;
}

void UAutonomousPopulationSubsystem::MakeDecisions()
{
	for (FCloneMindState& State : CloneStates)
	{
		ResolveIntent(State);
	}
}

void UAutonomousPopulationSubsystem::ResolveIntent(FCloneMindState& State)
{
	APawn* Pawn = State.Pawn.Get();
	if (!Pawn || Pawn->IsActorBeingDestroyed())
	{
		State.Intent = ECloneIntent::Idle;
		State.TargetPawn = nullptr;
		return;
	}

	if (State.bNightRaider)
	{
		if (const UDayNightSubsystem* DayNight = GetWorld() ? GetWorld()->GetSubsystem<UDayNightSubsystem>() : nullptr)
		{
			if (DayNight->GetCurrentPhase() != EDayPhase::Night)
			{
				State.Intent = ECloneIntent::Flee;
				State.TargetPawn = nullptr;
				return;
			}
		}
	}

	float Distance = 0.0f;
	APawn* NearbyPawn = FindNearestOtherPawn(Pawn, PerceptionRadius, Distance);
	if (!NearbyPawn)
	{
		State.Intent = ECloneIntent::Discover;
		State.TargetPawn = nullptr;
		OnCloneDiscovery.Broadcast(Pawn, Pawn->GetActorLocation());
		return;
	}

	const float KillIntent = ComputeKillIntent(State, NearbyPawn, Distance);
	if (KillIntent > 0.6f)
	{
		State.Intent = ECloneIntent::Hunt;
		State.TargetPawn = NearbyPawn;
		OnCloneConflict.Broadcast(Pawn, NearbyPawn);
		return;
	}

	const float FleeIntent = State.Desires.Paranoia + (1.0f - State.Desires.CombatPower * 0.5f);
	if (FleeIntent > 0.9f)
	{
		State.Intent = ECloneIntent::Flee;
		State.TargetPawn = NearbyPawn;
		return;
	}

	State.Intent = ECloneIntent::Discover;
	State.TargetPawn = nullptr;
	OnCloneDiscovery.Broadcast(Pawn, Pawn->GetActorLocation());
}

APawn* UAutonomousPopulationSubsystem::FindNearestOtherPawn(APawn* SourcePawn, const float MaxDistance, float& OutDistance) const
{
	OutDistance = MaxDistance;
	APawn* BestPawn = nullptr;

	for (const FCloneMindState& Other : CloneStates)
	{
		APawn* Candidate = Other.Pawn.Get();
		if (!Candidate || Candidate == SourcePawn || Candidate->IsActorBeingDestroyed())
		{
			continue;
		}

		const float Dist = FVector::Dist(SourcePawn->GetActorLocation(), Candidate->GetActorLocation());
		if (Dist < OutDistance)
		{
			OutDistance = Dist;
			BestPawn = Candidate;
		}
	}

	return BestPawn;
}

float UAutonomousPopulationSubsystem::ComputeKillIntent(const FCloneMindState& State, const APawn* Target, const float Distance) const
{
	if (!Target)
	{
		return 0.0f;
	}

	const float DistanceFactor = 1.0f - FMath::Clamp(Distance / FMath::Max(PerceptionRadius, 1.0f), 0.0f, 1.0f);
	const float AggressionPressure = State.Desires.Aggression * 0.55f;
	const float MercyReduction = State.Desires.Mercy * 0.35f;
	const float ParanoiaPressure = State.Desires.Paranoia * 0.2f;
	const float HealthPressure = (1.0f - State.HealthNormalized) * 0.15f;
	const float RivalryPressure = FMath::Clamp(HostilityByPair.FindRef(MakeRelationshipKey(State.Pawn.Get(), Target)), 0.0f, 1.0f) * 0.35f;

	return FMath::Clamp(BaseKillIntent + AggressionPressure + ParanoiaPressure + RivalryPressure + (DistanceFactor * 0.25f) - MercyReduction - HealthPressure, 0.0f, 1.0f);
}

void UAutonomousPopulationSubsystem::CleanupDeadPawns()
{
	for (int32 i = CloneStates.Num() - 1; i >= 0; --i)
	{
		APawn* Pawn = CloneStates[i].Pawn.Get();
		if (!Pawn || Pawn->IsActorBeingDestroyed() || CloneStates[i].HealthNormalized <= 0.0f)
		{
			if (Pawn && !Pawn->IsActorBeingDestroyed())
			{
				Pawn->Destroy();
			}
			CloneStates.RemoveAt(i);
		}
	}
}

void UAutonomousPopulationSubsystem::ResolveCombat()
{
	for (int32 i = 0; i < CloneStates.Num(); ++i)
	{
		FCloneMindState& AttackerState = CloneStates[i];
		APawn* Attacker = AttackerState.Pawn.Get();
		APawn* Target = AttackerState.TargetPawn.Get();
		if (!Attacker || !Target || AttackerState.Intent != ECloneIntent::Hunt)
		{
			continue;
		}

		const int32 TargetIndex = FindStateIndexByPawn(Target);
		if (TargetIndex == INDEX_NONE)
		{
			AttackerState.TargetPawn = nullptr;
			AttackerState.Intent = ECloneIntent::Discover;
			continue;
		}

		const float Distance = FVector::Dist(Attacker->GetActorLocation(), Target->GetActorLocation());
		if (Distance > CombatRange)
		{
			continue;
		}

		FCloneMindState& TargetState = CloneStates[TargetIndex];
		const float Damage = FMath::Clamp(DamagePerDecision * AttackerState.Desires.CombatPower, 0.01f, 1.0f);
		TargetState.HealthNormalized = FMath::Clamp(TargetState.HealthNormalized - Damage, 0.0f, 1.0f);

		const uint64 RelKey = MakeRelationshipKey(Attacker, Target);
		const float NewHostility = FMath::Clamp(HostilityByPair.FindRef(RelKey) + 0.12f, 0.0f, 1.0f);
		HostilityByPair.Add(RelKey, NewHostility);

		OnCloneConflict.Broadcast(Attacker, Target);

		if (TargetState.HealthNormalized <= 0.0f)
		{
			OnCloneKilled.Broadcast(Attacker, Target);
			AttackerState.TargetPawn = nullptr;
			AttackerState.Intent = ECloneIntent::Discover;
		}
	}
}

void UAutonomousPopulationSubsystem::ResolveNightRaiders()
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

	const bool bIsNight = DayNight->GetCurrentPhase() == EDayPhase::Night;
	if (bIsNight && !bWasNightLastTick)
	{
		int32 CurrentRaiders = 0;
		for (const FCloneMindState& State : CloneStates)
		{
			if (State.bNightRaider && State.Pawn.IsValid())
			{
				++CurrentRaiders;
			}
		}

		const int32 ToSpawn = FMath::Max(0, DesiredNightRaiderCount - CurrentRaiders);
		const APlayerController* PC = World->GetFirstPlayerController();
		const APawn* PlayerPawn = PC ? PC->GetPawn() : nullptr;
		const FVector Center = PlayerPawn ? PlayerPawn->GetActorLocation() : FVector::ZeroVector;

		for (int32 i = 0; i < ToSpawn; ++i)
		{
			const FVector Offset = FMath::VRand() * FMath::FRandRange(1500.0f, 5000.0f);
			if (APawn* Raider = SpawnCloneAtLocation(Center + FVector(Offset.X, Offset.Y, 120.0f)))
			{
				const int32 Idx = FindStateIndexByPawn(Raider);
				if (Idx != INDEX_NONE)
				{
					FCloneMindState& State = CloneStates[Idx];
					State.bNightRaider = true;
					State.Desires.Aggression = 1.0f;
					State.Desires.Paranoia = FMath::Max(State.Desires.Paranoia, 0.65f);
					State.Desires.Mercy = 0.0f;
				}
			}
		}
	}

	if (!bIsNight)
	{
		for (FCloneMindState& State : CloneStates)
		{
			if (!State.bNightRaider)
			{
				continue;
			}

			APawn* RaiderPawn = State.Pawn.Get();
			if (!RaiderPawn || RaiderPawn->IsActorBeingDestroyed())
			{
				continue;
			}

			if (!IsShelteredFromSun(RaiderPawn))
			{
				State.HealthNormalized = FMath::Clamp(State.HealthNormalized - DaylightDamagePerDecision, 0.0f, 1.0f);
			}
		}
	}

	bWasNightLastTick = bIsNight;
}

int32 UAutonomousPopulationSubsystem::FindStateIndexByPawn(const APawn* Pawn) const
{
	if (!Pawn)
	{
		return INDEX_NONE;
	}

	for (int32 i = 0; i < CloneStates.Num(); ++i)
	{
		if (CloneStates[i].Pawn.Get() == Pawn)
		{
			return i;
		}
	}

	return INDEX_NONE;
}

uint64 UAutonomousPopulationSubsystem::MakeRelationshipKey(const APawn* A, const APawn* B) const
{
	if (!A || !B)
	{
		return 0;
	}

	const uint32 AId = static_cast<uint32>(A->GetUniqueID());
	const uint32 BId = static_cast<uint32>(B->GetUniqueID());
	const uint32 Low = FMath::Min(AId, BId);
	const uint32 High = FMath::Max(AId, BId);
	return (static_cast<uint64>(Low) << 32) | static_cast<uint64>(High);
}

bool UAutonomousPopulationSubsystem::IsShelteredFromSun(const APawn* Pawn) const
{
	if (!Pawn)
	{
		return false;
	}

	TArray<AActor*> OverlappingActors;
	Pawn->GetOverlappingActors(OverlappingActors);
	for (const AActor* Actor : OverlappingActors)
	{
		if (!Actor)
		{
			continue;
		}

		if (Actor->ActorHasTag(TEXT("Shade")) || Actor->ActorHasTag(TEXT("Tunnel")) || Actor->ActorHasTag(TEXT("SunShelter")))
		{
			return true;
		}

		if (Actor->GetClass()->GetName().Contains(TEXT("SafeHouseVolume")))
		{
			return true;
		}
	}

	return false;
}
