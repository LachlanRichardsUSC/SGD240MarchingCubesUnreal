// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;


public class SGD240Procedural : ModuleRules
{
	public SGD240Procedural(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "ProceduralMeshComponent" });
		
		
	}
}
