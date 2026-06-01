#include "ResourceGenerationPresetLibrary.h"

TArray<FResourcePresetOption> UResourceGenerationPresetLibrary::GetPresetOptions()
{
	return {
		{EResourceGenerationPreset::BalancedFrontier, FText::FromString(TEXT("Balanced Frontier"))},
		{EResourceGenerationPreset::ScarcitySurvival, FText::FromString(TEXT("Scarcity Survival"))},
		{EResourceGenerationPreset::IndustrialRich, FText::FromString(TEXT("Industrial Rich"))},
		{EResourceGenerationPreset::ExplorationWilds, FText::FromString(TEXT("Exploration Wilds"))},
		{EResourceGenerationPreset::LandmarkTreasure, FText::FromString(TEXT("Landmark Treasure"))}
	};
}

void UResourceGenerationPresetLibrary::ApplyPresetToProfile(const EResourceGenerationPreset Preset, UResourceGenerationProfile* Profile)
{
	if (!Profile)
	{
		return;
	}

	Profile->ResourceRules.Empty();

	switch (Preset)
	{
	case EResourceGenerationPreset::BalancedFrontier:
		Profile->ResourceSeed = 7001;
		Profile->DistributionMode = EResourceDistributionMode::Clustered;
		Profile->GlobalDensity = 0.5f;
		Profile->ClusterTightness = 0.55f;
		Profile->RespawnIntervalMinutes = 20.0f;
		Profile->ResourceRules = {
			{FName("Wood"), EResourceRarityTier::Common, 0.35f, 120, 260, EResourceBiomeAffinity::Any},
			{FName("Stone"), EResourceRarityTier::Common, 0.30f, 90, 200, EResourceBiomeAffinity::Any},
			{FName("Iron"), EResourceRarityTier::Uncommon, 0.20f, 50, 120, EResourceBiomeAffinity::Mountain},
			{FName("Herbs"), EResourceRarityTier::Uncommon, 0.15f, 60, 140, EResourceBiomeAffinity::Temperate}
		};
		break;

	case EResourceGenerationPreset::ScarcitySurvival:
		Profile->ResourceSeed = 7002;
		Profile->DistributionMode = EResourceDistributionMode::VeinBased;
		Profile->GlobalDensity = 0.22f;
		Profile->ClusterTightness = 0.8f;
		Profile->RespawnIntervalMinutes = 45.0f;
		Profile->DepletionRecoveryFactor = 0.4f;
		Profile->ResourceRules = {
			{FName("Wood"), EResourceRarityTier::Common, 0.30f, 40, 100, EResourceBiomeAffinity::Any},
			{FName("Stone"), EResourceRarityTier::Common, 0.30f, 35, 90, EResourceBiomeAffinity::Any},
			{FName("Iron"), EResourceRarityTier::Uncommon, 0.25f, 20, 60, EResourceBiomeAffinity::Mountain},
			{FName("Crystal"), EResourceRarityTier::Rare, 0.15f, 6, 20, EResourceBiomeAffinity::Snow}
		};
		break;

	case EResourceGenerationPreset::IndustrialRich:
		Profile->ResourceSeed = 7003;
		Profile->DistributionMode = EResourceDistributionMode::Uniform;
		Profile->GlobalDensity = 0.72f;
		Profile->ClusterTightness = 0.35f;
		Profile->RespawnIntervalMinutes = 12.0f;
		Profile->ResourceRules = {
			{FName("Coal"), EResourceRarityTier::Common, 0.25f, 120, 300, EResourceBiomeAffinity::Any},
			{FName("Iron"), EResourceRarityTier::Uncommon, 0.30f, 100, 220, EResourceBiomeAffinity::Mountain},
			{FName("Copper"), EResourceRarityTier::Uncommon, 0.25f, 90, 210, EResourceBiomeAffinity::Any},
			{FName("Oil"), EResourceRarityTier::Rare, 0.20f, 30, 90, EResourceBiomeAffinity::Coastal}
		};
		break;

	case EResourceGenerationPreset::ExplorationWilds:
		Profile->ResourceSeed = 7004;
		Profile->DistributionMode = EResourceDistributionMode::Clustered;
		Profile->GlobalDensity = 0.42f;
		Profile->ClusterTightness = 0.65f;
		Profile->LandmarkBias = 0.4f;
		Profile->RespawnIntervalMinutes = 25.0f;
		Profile->ResourceRules = {
			{FName("Herbs"), EResourceRarityTier::Common, 0.30f, 100, 240, EResourceBiomeAffinity::Temperate},
			{FName("Berries"), EResourceRarityTier::Common, 0.25f, 90, 220, EResourceBiomeAffinity::Temperate},
			{FName("AncientRelic"), EResourceRarityTier::Rare, 0.12f, 10, 30, EResourceBiomeAffinity::Any},
			{FName("Crystal"), EResourceRarityTier::Epic, 0.08f, 4, 12, EResourceBiomeAffinity::Mountain},
			{FName("Stone"), EResourceRarityTier::Uncommon, 0.25f, 60, 140, EResourceBiomeAffinity::Any}
		};
		break;

	case EResourceGenerationPreset::LandmarkTreasure:
		Profile->ResourceSeed = 7005;
		Profile->DistributionMode = EResourceDistributionMode::LandmarkBiased;
		Profile->GlobalDensity = 0.35f;
		Profile->ClusterTightness = 0.75f;
		Profile->LandmarkBias = 0.9f;
		Profile->RespawnIntervalMinutes = 60.0f;
		Profile->DepletionRecoveryFactor = 0.3f;
		Profile->ResourceRules = {
			{FName("GoldVein"), EResourceRarityTier::Rare, 0.30f, 8, 24, EResourceBiomeAffinity::Mountain},
			{FName("AncientRelic"), EResourceRarityTier::Rare, 0.30f, 8, 24, EResourceBiomeAffinity::Any},
			{FName("Crystal"), EResourceRarityTier::Epic, 0.20f, 3, 10, EResourceBiomeAffinity::Snow},
			{FName("Pearl"), EResourceRarityTier::Epic, 0.20f, 3, 10, EResourceBiomeAffinity::Coastal}
		};
		break;

	default:
		break;
	}
}
