#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "KineticMagiDebugBPLibrary.generated.h"

class UAutonomousPopulationSubsystem;
class UDayNightSubsystem;
class UKineticMagiDebugSubsystem;

UCLASS()
class KINETICMAGI_API UKineticMagiDebugBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "KM|Debug", meta = (WorldContext = "WorldContextObject"))
	static void KM_SetTimeNight(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "KM|Debug", meta = (WorldContext = "WorldContextObject"))
	static void KM_SetTimeDay(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "KM|Debug", meta = (WorldContext = "WorldContextObject"))
	static void KM_SpawnCloneWave(UObject* WorldContextObject, int32 Count, float Radius);

	UFUNCTION(BlueprintCallable, Category = "KM|Debug", meta = (WorldContext = "WorldContextObject"))
	static void KM_SetDebugOverlayEnabled(UObject* WorldContextObject, bool bEnabled);
};
