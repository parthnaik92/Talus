// Copyright (c) 2026 parthnaik92. Licensed under the MIT License. See LICENSE.

using UnrealBuildTool;

public class Talus : ModuleRules
{
	public Talus(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"RenderCore", // Shader directory mapping, texture updates
			"RHI",        // (M2+) compute shader dispatch
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Projects", // IPluginManager: locate the plugin's Shaders/ directory
			"Renderer", // (M2+) RDG compute passes
		});
	}
}
