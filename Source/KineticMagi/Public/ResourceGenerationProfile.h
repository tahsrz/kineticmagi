#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ResourceGenerationProfile.generated.h"

UENUM(BlueprintType)
enum class EResourceBiomeAffinity : uint8
{
	Any,
	Temperate,
	Coastal,
	Desert,
	Mountain,
	Snow
};

UENUM(BlueprintType)
enum class EResourceRarityTier : uint8
{
	Common,
	Uncommon,
	Rare,
	Epic
};

UENUM(BlueprintType)
enum class EResourceDistributionMode : uint8
{
	Uniform,
	Clustered,
	VeinBased,
	LandmarkBiased
};

USTRUCT(BlueprintType)
struct FResourceNodeRule
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resources")
	FName ResourceId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resources")
	EResourceRarityTier Rarity = EResourceRarityTier::Common;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resources", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float SpawnWeight = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resources", meta = (ClampMin = "0", ClampMax = "100000"))
	int32 MinNodeCount = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resources", meta = (ClampMin = "0", ClampMax = "100000"))
	int32 MaxNodeCount = 50;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resources")
	EResourceBiomeAffinity BiomeAffinity = EResourceBiomeAffinity::Any;
};

UCLASS(BlueprintType)
class KINETICMAGI_API UResourceGenerationProfile : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resources|Core")
	int32 ResourceSeed = 777;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resources|Core")
	EResourceDistributionMode DistributionMode = EResourceDistributionMode::Clustered;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resources|Core", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float GlobalDensity = 0.45f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resources|Core", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ClusterTightness = 0.55f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resources|Core", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float LandmarkBias = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resources|Respawn", meta = (ClampMin = "0.0"))
	float RespawnIntervalMinutes = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resources|Respawn", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DepletionRecoveryFactor = 0.65f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resources|Rules")
	TArray<FResourceNodeRule> ResourceRules;
};
