#include "TownGenerationPresetLibrary.h"

TArray<FTownPresetOption> UTownGenerationPresetLibrary::GetPresetOptions()
{
	return {
		{ETownGenerationPreset::RiversideMarket, FText::FromString(TEXT("Riverside Market"))},
		{ETownGenerationPreset::IndustrialBasin, FText::FromString(TEXT("Industrial Basin"))},
		{ETownGenerationPreset::GardenDistrict, FText::FromString(TEXT("Garden District"))},
		{ETownGenerationPreset::OldQuarter, FText::FromString(TEXT("Old Quarter"))},
		{ETownGenerationPreset::NeonMetro, FText::FromString(TEXT("Neon Metro"))}
	};
}

void UTownGenerationPresetLibrary::ApplyPresetToProfile(const ETownGenerationPreset Preset, UTownGenerationProfile* Profile)
{
	if (!Profile)
	{
		return;
	}

	switch (Preset)
	{
	case ETownGenerationPreset::RiversideMarket:
		Profile->TownSeed = 1107;
		Profile->BiomeType = ETownBiomeType::Temperate;
		Profile->WaterProximity = ETownWaterProximity::RiverAdjacent;
		Profile->RoadGraphType = ETownRoadGraphType::Organic;
		Profile->DistrictCount = 7;
		Profile->ParkCoverage = 0.18f;
		Profile->ShopfrontFrequency = 0.55f;
		Profile->NpcFootTrafficDensity = 0.70f;
		Profile->VehicleSpawnDensity = 0.30f;
		Profile->AmbientSoundProfile = ETownAmbientSoundProfile::BusyUrban;
		break;

	case ETownGenerationPreset::IndustrialBasin:
		Profile->TownSeed = 2219;
		Profile->BiomeType = ETownBiomeType::Coastal;
		Profile->WaterProximity = ETownWaterProximity::LakeAdjacent;
		Profile->RoadGraphType = ETownRoadGraphType::Grid;
		Profile->DistrictCount = 5;
		Profile->IndustrialPresence = 0.55f;
		Profile->CivicBuildingFrequency = 0.06f;
		Profile->DecorativePropDensity = 0.25f;
		Profile->FogHazeLevel = 0.35f;
		Profile->AmbientSoundProfile = ETownAmbientSoundProfile::Industrial;
		break;

	case ETownGenerationPreset::GardenDistrict:
		Profile->TownSeed = 3141;
		Profile->BiomeType = ETownBiomeType::Temperate;
		Profile->RoadGraphType = ETownRoadGraphType::Hybrid;
		Profile->DistrictCount = 6;
		Profile->ParkCoverage = 0.35f;
		Profile->TreeDensity = 0.80f;
		Profile->ResidentialCommercialRatio = 0.78f;
		Profile->BuildingHeightMax = 4;
		Profile->VehicleSpawnDensity = 0.20f;
		Profile->AmbientSoundProfile = ETownAmbientSoundProfile::Suburban;
		break;

	case ETownGenerationPreset::OldQuarter:
		Profile->TownSeed = 4412;
		Profile->EraStyle = ETownEraStyle::Industrial;
		Profile->ArchitectureSet = ETownArchitectureSet::NeoGothic;
		Profile->RoadGraphType = ETownRoadGraphType::Organic;
		Profile->AlleyFrequency = 0.65f;
		Profile->CourtyardFrequency = 0.40f;
		Profile->BuildingAgeVariance = 0.85f;
		Profile->ConditionLevel = ETownBuildingCondition::Aged;
		Profile->ShopfrontFrequency = 0.40f;
		Profile->RareSpecialArtifactChance = 0.08f;
		break;

	case ETownGenerationPreset::NeonMetro:
		Profile->TownSeed = 5599;
		Profile->EraStyle = ETownEraStyle::Futuristic;
		Profile->ArchitectureSet = ETownArchitectureSet::ConcreteModern;
		Profile->WealthTier = ETownWealthTier::High;
		Profile->RoadGraphType = ETownRoadGraphType::Grid;
		Profile->DistrictCount = 9;
		Profile->BuildingHeightMax = 14;
		Profile->StreetLightDensity = 0.95f;
		Profile->NightLightingTemperature = 5200.0f;
		Profile->NpcFootTrafficDensity = 0.85f;
		break;

	default:
		break;
	}
}
