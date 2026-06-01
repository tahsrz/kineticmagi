#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "NightEnemyDirectorSubsystem.generated.h"

UENUM(BlueprintType)
enum class ENightThreatLevel : uint8
{
	Calm,
	Hunting,
	Alerted,
	Overrun
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnThreatLevelChanged, ENightThreatLevel, NewLevel);

UCLASS()
class KINETICMAGI_API UNightEnemyDirectorSubsystem : public UWorldSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override;
	virtual UWorld* GetTickableGameObjectWorld() const override;

	UFUNCTION(BlueprintPure, Category = "NightThreat")
	bool IsNightAssaultActive() const { return bNightAssaultActive; }

	UFUNCTION(BlueprintPure, Category = "NightThreat")
	ENightThreatLevel GetCurrentThreatLevel() const { return CurrentThreatLevel; }

	UFUNCTION(BlueprintPure, Category = "NightThreat")
	float GetSpawnPressure() const { return SpawnPressure; }

	UPROPERTY(BlueprintAssignable, Category = "NightThreat")
	FOnThreatLevelChanged OnThreatLevelChanged;

private:
	ENightThreatLevel ResolveThreatLevel(float Suspicion) const;

	bool bNightAssaultActive = false;
	ENightThreatLevel CurrentThreatLevel = ENightThreatLevel::Calm;
	float SpawnPressure = 0.0f;
};
