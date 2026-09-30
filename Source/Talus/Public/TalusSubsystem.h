// Copyright (c) 2026 parthnaik92. Licensed under the MIT License. See LICENSE.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TalusSubsystem.generated.h"

class UTalusHeightfield;

/**
 * Front door to Talus. Auto-created per world; manages heightfield lifetime
 * and (from M2 on) owns the async GPU generation state, so callers never deal
 * with threading or RHI lifetimes directly.
 *
 * Reach it from C++ via World->GetSubsystem<UTalusSubsystem>(), or from
 * Blueprints via the Talus Blueprint Function Library.
 */
UCLASS(BlueprintType)
class TALUS_API UTalusSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	/**
	 * Creates a new heightfield of Size x Size. The subsystem owns it (safe
	 * from garbage collection); the caller just holds the pointer.
	 * M1: filled with the analytic test pattern. M2+: runs the GPU generators.
	 */
	UFUNCTION(BlueprintCallable, Category = "Talus")
	UTalusHeightfield* CreateHeightfield(int32 Size = 1024);

	/**
	 * Returns the shared heightfield registered under Name, creating it on
	 * first use (or recreating it if the requested size changed). Useful when
	 * several systems need "the" terrain heightfield without passing object
	 * references around.
	 */
	UFUNCTION(BlueprintCallable, Category = "Talus")
	UTalusHeightfield* GetOrCreateSharedHeightfield(FName Name, int32 Size = 1024);

protected:
	virtual void Deinitialize() override;

private:
	UPROPERTY(Transient)
	TMap<FName, TObjectPtr<UTalusHeightfield>> SharedHeightfields;
};
