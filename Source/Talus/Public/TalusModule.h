// Copyright (c) 2026 parthnaik92. Licensed under the MIT License. See LICENSE.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTalus, Log, All);

class FTalusModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
