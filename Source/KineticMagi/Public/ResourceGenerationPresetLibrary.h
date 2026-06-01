#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ResourceGenerationProfile.h"
#include "ResourceGenerationPresetLibrary.generated.h"

UENUM(BlueprintType)
enum class EResourceGenerationPreset : uint8
{
	BalancedFrontier,
	ScarcitySurvival,
	IndustrialRich,
	ExplorationWilds,
	LandmarkTreasure
};

USTRUCT(BlueprintType)
struct FResourcePresetOption
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resources|Presets")
	EResourceGenerationPreset Preset = EResourceGenerationPreset::BalancedFrontier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resources|Presets")
	FText DisplayName;
};

UCLASS()
class KINETICMAGI_API UResourceGenerationPresetLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Resources|Presets")
	static TArray<FResourcePresetOption> GetPresetOptions();

	UFUNCTION(BlueprintCallable, Category = "Resources|Presets")
	static void ApplyPresetToProfile(EResourceGenerationPreset Preset, UResourceGenerationProfile* Profile);
};
