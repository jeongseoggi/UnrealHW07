// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UEHW07 : ModuleRules
{
	public UEHW07(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
