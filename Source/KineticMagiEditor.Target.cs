using UnrealBuildTool;
using System.Collections.Generic;

public class KineticMagiEditorTarget : TargetRules
{
	public KineticMagiEditorTarget(TargetInfo Target) : base(Target)
{
    Type = TargetType.Editor;
    DefaultBuildSettings = BuildSettingsVersion.V6;
    IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
    BuildEnvironment = TargetBuildEnvironment.Shared; 
    ExtraModuleNames.Add("KineticMagi");
}
}
