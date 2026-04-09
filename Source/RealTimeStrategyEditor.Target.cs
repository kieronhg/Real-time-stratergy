using UnrealBuildTool;
using System.Collections.Generic;

public class RealTimeStrategyEditorTarget : TargetRules
{
	public RealTimeStrategyEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;
		ExtraModuleNames.Add("RealTimeStrategy");
	}
}
