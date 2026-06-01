#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TownGenerationProfile.h"
#include "TownGenerationPresetLibrary.generated.h"

UENUM(BlueprintType)
enum class ETownGenerationPreset : uint8
{
	RiversideMarket,
	IndustrialBasin,
	GardenDistrict,
	OldQuarter,
	NeonMetro
};

USTRUCT(BlueprintType)
struct FTownPresetOption
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Presets")
	ETownGenerationPreset Preset = ETownGenerationPreset::RiversideMarket;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Presets")
	FText DisplayName;
};

UCLASS()
class KINETICMAGI_API UTownGenerationPresetLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Town|Presets")
	static TArray<FTownPresetOption> GetPresetOptions();

	UFUNCTION(BlueprintCallable, Category = "Town|Presets")
	static void ApplyPresetToProfile(ETownGenerationPreset Preset, UTownGenerationProfile* Profile);
};
