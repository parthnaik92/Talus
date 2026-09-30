// Copyright (c) 2026 parthnaik92. Licensed under the MIT License. See LICENSE.

#include "TalusModule.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "ShaderCore.h"

#define LOCTEXT_NAMESPACE "FTalusModule"

DEFINE_LOG_CATEGORY(LogTalus);

void FTalusModule::StartupModule()
{
	// Map the virtual shader path /Plugin/Talus/ to this plugin's Shaders/
	// directory so .usf files shipped with the plugin are visible to the
	// shader compiler. Used from milestone 2 onward for compute kernels.
	const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("Talus"));
	if (Plugin.IsValid())
	{
		const FString ShaderDir = FPaths::Combine(Plugin->GetBaseDir(), TEXT("Shaders"));
		AddShaderSourceDirectoryMapping(TEXT("/Plugin/Talus"), ShaderDir);
		UE_LOG(LogTalus, Log, TEXT("Mapped /Plugin/Talus/ -> %s"), *ShaderDir);
	}
	else
	{
		UE_LOG(LogTalus, Warning, TEXT("Could not find Talus plugin; shader directory not mapped."));
	}
}

void FTalusModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FTalusModule, Talus)
