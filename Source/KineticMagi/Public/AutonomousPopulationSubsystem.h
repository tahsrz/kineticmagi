#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AutonomousPopulationSubsystem.generated.h"

UENUM(BlueprintType)
enum class ECloneIntent : uint8
{
	Idle,
	Discover,
	Hunt,
	Flee
};

USTRUCT(BlueprintType)
struct FCloneDesireProfile
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clones", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Aggression = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clones", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DiscoveryDrive = 0.7f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clones", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Paranoia = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clones", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Mercy = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clones", meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float CombatPower = 1.0f;
};

USTRUCT(BlueprintType)
struct FCloneMindState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Clones")
	TObjectPtr<APawn> Pawn = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Clones")
	FCloneDesireProfile Desires;

	UPROPERTY(BlueprintReadOnly, Category = "Clones")
	ECloneIntent Intent = ECloneIntent::Idle;

	UPROPERTY(BlueprintReadOnly, Category = "Clones")
	TObjectPtr<APawn> TargetPawn = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Clones", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float HealthNormalized = 1.0f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCloneConflict, APawn*, Attacker, APawn*, Target);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCloneDiscovery, APawn*, Discoverer, FVector, Location);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCloneKilled, APawn*, Killer, APawn*, Victim);

UCLASS()
class KINETICMAGI_API UAutonomousPopulationSubsystem : public UWorldSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override;
	virtual UWorld* GetTickableGameObjectWorld() const override;

	UFUNCTION(BlueprintCallable, Category = "Clones")
	void InitializeCloneClassFromPlayer();

	UFUNCTION(BlueprintCallable, Category = "Clones")
	void SpawnInitialPopulation(int32 Count, float Radius = 2500.0f);

	UFUNCTION(BlueprintCallable, Category = "Clones")
	APawn* SpawnCloneAtLocation(const FVector& Location);

	UFUNCTION(BlueprintPure, Category = "Clones")
	const TArray<FCloneMindState>& GetCloneStates() const { return CloneStates; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clones")
	TSubclassOf<APawn> ClonePawnClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clones", meta = (ClampMin = "0.1", ClampMax = "10.0"))
	float DecisionIntervalSeconds = 1.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clones", meta = (ClampMin = "100.0", ClampMax = "100000.0"))
	float PerceptionRadius = 1800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clones", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float BaseKillIntent = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clones|Combat", meta = (ClampMin = "50.0", ClampMax = "5000.0"))
	float CombatRange = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clones|Combat", meta = (ClampMin = "0.01", ClampMax = "1.0"))
	float DamagePerDecision = 0.2f;

	UPROPERTY(BlueprintAssignable, Category = "Clones")
	FOnCloneConflict OnCloneConflict;

	UPROPERTY(BlueprintAssignable, Category = "Clones")
	FOnCloneDiscovery OnCloneDiscovery;

	UPROPERTY(BlueprintAssignable, Category = "Clones")
	FOnCloneKilled OnCloneKilled;

private:
	void MakeDecisions();
	void ResolveIntent(FCloneMindState& State);
	void ResolveCombat();
	APawn* FindNearestOtherPawn(APawn* SourcePawn, float MaxDistance, float& OutDistance) const;
	float ComputeKillIntent(const FCloneMindState& State, const APawn* Target, float Distance) const;
	int32 FindStateIndexByPawn(const APawn* Pawn) const;
	uint64 MakeRelationshipKey(const APawn* A, const APawn* B) const;
	void CleanupDeadPawns();

	float DecisionAccumulator = 0.0f;

	UPROPERTY()
	TArray<FCloneMindState> CloneStates;

	TMap<uint64, float> HostilityByPair;
};
