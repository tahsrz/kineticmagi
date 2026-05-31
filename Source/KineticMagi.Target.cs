using UnrealBuildTool;
using System.Collections.Generic;

public class KineticMagiTarget : TargetRules
{
	public KineticMagiTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("KineticMagi");
	}
}
