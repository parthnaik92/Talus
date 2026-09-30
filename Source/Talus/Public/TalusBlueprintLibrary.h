// Copyright (c) 2026 parthnaik92. Licensed under the MIT License. See LICENSE.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TalusBlueprintLibrary.generated.h"

class UTalusHeightfield;
class UTalusSubsystem;

/**
 * Convenience Blueprint nodes for Talus. Everything here forwards to
 * UTalusSubsystem, which does the real work.
 */
UCLASS()
class TALUS_API UTalusBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Returns the Talus subsystem for the world containing WorldContextObject. */
	UFUNCTION(BlueprintPure, Category = "Talus", meta = (WorldContext = "WorldContextObject"))
	static UTalusSubsystem* GetTalusSubsystem(const UObject* WorldContextObject);

	/**
	 * Creates a heightfield of Size x Size, ready to use.
	 * M1: filled with the analytic test pattern. M2+: runs the GPU generators.
	 */
	UFUNCTION(BlueprintCallable, Category = "Talus", meta = (WorldContext = "WorldContextObject"))
	static UTalusHeightfield* CreateTalusHeightfield(const UObject* WorldContextObject, int32 Size = 1024);
};
