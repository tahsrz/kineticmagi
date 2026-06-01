#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TownGenerationProfile.generated.h"

UENUM(BlueprintType)
enum class ETownBiomeType : uint8
{
	Temperate,
	Coastal,
	Desert,
	Mountain,
	Snow
};

UENUM(BlueprintType)
enum class ETownElevationProfile : uint8
{
	Flat,
	Rolling,
	Hilly,
	Steep
};

UENUM(BlueprintType)
enum class ETownWaterProximity : uint8
{
	Inland,
	RiverAdjacent,
	LakeAdjacent,
	Coastal
};

UENUM(BlueprintType)
enum class ETownClimateZone : uint8
{
	Arid,
	Temperate,
	Tropical,
	Cold
};

UENUM(BlueprintType)
enum class ETownEraStyle : uint8
{
	Medieval,
	Industrial,
	Modern,
	Futuristic
};

UENUM(BlueprintType)
enum class ETownArchitectureSet : uint8
{
	Woodland,
	BrickUrban,
	ConcreteModern,
	Mediterranean,
	NeoGothic
};

UENUM(BlueprintType)
enum class ETownWealthTier : uint8
{
	Low,
	Mid,
	High,
	Luxury
};

UENUM(BlueprintType)
enum class ETownRoadGraphType : uint8
{
	Grid,
	Radial,
	Organic,
	Hybrid
};

UENUM(BlueprintType)
enum class ETownDistrictType : uint8
{
	Residential,
	Commercial,
	Industrial,
	Civic,
	Market,
	Harbor
};

UENUM(BlueprintType)
enum class ETownBuildingCondition : uint8
{
	Pristine,
	Worn,
	Aged,
	Derelict
};

UENUM(BlueprintType)
enum class ETownAmbientSoundProfile : uint8
{
	Quiet,
	Suburban,
	BusyUrban,
	Industrial
};

USTRUCT(BlueprintType)
struct FTownDistrictWeight
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Districts")
	ETownDistrictType DistrictType = ETownDistrictType::Residential;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Districts", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Weight = 0.25f;
};

UCLASS(BlueprintType)
class KINETICMAGI_API UTownGenerationProfile : public UDataAsset
{
	GENERATED_BODY()

public:
	// 1-10 Core
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Core")
	int32 TownSeed = 1337;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Core")
	ETownBiomeType BiomeType = ETownBiomeType::Temperate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Core")
	ETownElevationProfile ElevationProfile = ETownElevationProfile::Rolling;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Core")
	ETownWaterProximity WaterProximity = ETownWaterProximity::RiverAdjacent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Core")
	ETownClimateZone ClimateZone = ETownClimateZone::Temperate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Core")
	ETownEraStyle EraStyle = ETownEraStyle::Modern;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Core")
	ETownArchitectureSet ArchitectureSet = ETownArchitectureSet::BrickUrban;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Core")
	ETownWealthTier WealthTier = ETownWealthTier::Mid;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Core", meta = (ClampMin = "1"))
	int32 PopulationTarget = 4000;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Core", meta = (ClampMin = "1", ClampMax = "20"))
	int32 DistrictCount = 6;

	// 11-20 Layout
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Layout")
	TArray<FTownDistrictWeight> DistrictTypeMix;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Layout")
	ETownRoadGraphType RoadGraphType = ETownRoadGraphType::Hybrid;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Layout", meta = (ClampMin = "200.0", ClampMax = "4000.0"))
	float MainRoadWidth = 900.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Layout", meta = (ClampMin = "100.0", ClampMax = "2000.0"))
	float SideStreetWidth = 450.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Layout", meta = (ClampMin = "1000.0", ClampMax = "25000.0"))
	float BlockSizeMin = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Layout", meta = (ClampMin = "1000.0", ClampMax = "25000.0"))
	float BlockSizeMax = 12000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Layout", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float IntersectionDensity = 0.6f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Layout", meta = (ClampMin = "0", ClampMax = "20"))
	int32 LandmarkCount = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Layout", meta = (ClampMin = "0", ClampMax = "20"))
	int32 PlazaCount = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Layout", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ParkCoverage = 0.15f;

	// 21-30 Streetscape + Building Form
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Streetscape", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float TreeDensity = 0.45f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Streetscape", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float StreetLightDensity = 0.7f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Streetscape", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float SidewalkStyle = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Streetscape", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float FenceStyleSet = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Streetscape", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float UtilityPoleDensity = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Streetscape", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float SignageStyle = 0.6f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Buildings", meta = (ClampMin = "1", ClampMax = "50"))
	int32 BuildingHeightMin = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Buildings", meta = (ClampMin = "1", ClampMax = "100"))
	int32 BuildingHeightMax = 6;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Buildings", meta = (ClampMin = "200.0", ClampMax = "20000.0"))
	float BuildingFootprintMin = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Buildings", meta = (ClampMin = "200.0", ClampMax = "40000.0"))
	float BuildingFootprintMax = 4500.0f;

	// 31-40 Building Character + City Composition
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Buildings", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float RoofTypeDistribution = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Buildings", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float FacadeMaterialPalette = 0.55f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Buildings", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float WindowStyleDistribution = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Buildings", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DoorStyleDistribution = 0.45f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Buildings", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float BalconyChance = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Buildings", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float AwningChance = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Buildings", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ShopfrontFrequency = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Buildings", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ResidentialCommercialRatio = 0.65f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Buildings", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float IndustrialPresence = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Buildings", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float CivicBuildingFrequency = 0.08f;

	// 41-50 Atmosphere + Activity
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Atmosphere", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float BuildingAgeVariance = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Atmosphere")
	ETownBuildingCondition ConditionLevel = ETownBuildingCondition::Worn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Atmosphere", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ColorPaletteProfile = 0.55f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Atmosphere", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DecorativePropDensity = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Atmosphere", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float AlleyFrequency = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Atmosphere", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float CourtyardFrequency = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Atmosphere", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ParkingLotFrequency = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Atmosphere", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float VehicleSpawnDensity = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Atmosphere", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float NpcFootTrafficDensity = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Atmosphere")
	ETownAmbientSoundProfile AmbientSoundProfile = ETownAmbientSoundProfile::Suburban;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Atmosphere", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float FogHazeLevel = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Atmosphere", meta = (ClampMin = "1000.0", ClampMax = "12000.0"))
	float NightLightingTemperature = 3400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town|Atmosphere", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float RareSpecialArtifactChance = 0.03f;
};
