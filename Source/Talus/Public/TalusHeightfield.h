// Copyright (c) 2026 parthnaik92. Licensed under the MIT License. See LICENSE.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TalusHeightfield.generated.h"

class UTexture2D;

/**
 * Owns the heightfield texture that Talus generates.
 *
 * Milestone 1: creates a transient R32F texture and can fill it with a simple
 * analytic test pattern, so the Talus -> texture -> terrain renderer
 * (e.g. Shader World) plumbing can be validated before any real generation
 * exists. Later milestones replace FillTestPattern() with GPU compute
 * generators; the texture contract stays the same.
 */
UCLASS(BlueprintType)
class TALUS_API UTalusHeightfield : public UObject
{
	GENERATED_BODY()

public:
	/** (Re)creates the heightmap texture at Size x Size. Existing contents are discarded. */
	UFUNCTION(BlueprintCallable, Category = "Talus")
	void Initialize(int32 Size = 1024);

	/**
	 * Fills the heightmap with a simple radial test pattern (values 0..1).
	 * Temporary scaffolding for milestone 1; replaced by real generators in M2+.
	 */
	UFUNCTION(BlueprintCallable, Category = "Talus")
	void FillTestPattern();

	/**
	 * Read-only access to the texture. Bind this into your terrain material
	 * (e.g. Shader World's generator material, sampled by world XZ) to
	 * render the generated heightfield.
	 */
	UFUNCTION(BlueprintPure, Category = "Talus")
	UTexture2D* GetHeightmapTexture() const { return HeightmapTexture; }

	/** Current texture resolution (Size x Size). 0 until Initialize() is called. */
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Talus")
	int32 HeightmapSize = 0;

private:
	UPROPERTY(Transient)
	TObjectPtr<UTexture2D> HeightmapTexture = nullptr;
};
