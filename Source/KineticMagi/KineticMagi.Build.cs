using UnrealBuildTool;

public class KineticMagi : ModuleRules
{
	public KineticMagi(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"GameplayTags", 
			"Mover", 
			"NetworkPrediction", 
			"ChaosMover",
			"Sockets",
			"Networking"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });
	}
}

