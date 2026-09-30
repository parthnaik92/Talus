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
 * exists. Milestone 2: GPU compute generators (starting with FBM noise);
 * the texture contract stays the same. FillTestPattern() remains as a debug
 * utility to verify the texture path independently of the kernels.
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
	 * Debug utility: verifies the texture path independently of the noise kernels.
	 */
	UFUNCTION(BlueprintCallable, Category = "Talus")
	void FillTestPattern();

	/**
	 * Generates FBM noise into the heightfield on the GPU.
	 * Parameters mirror wgen's FbmConf: MulX/MulY zoom the noise (higher =
	 * smaller features), AddX/AddY slide the sample window, Octaves layers
	 * detail, Delta offsets the base height, Scale sets the bump height.
	 * Dispatches on the render thread; the texture updates in place.
	 */
	UFUNCTION(BlueprintCallable, Category = "Talus")
	void GenerateFbm(int32 Seed = 1337, float MulX = 2.2f, float MulY = 2.2f, float AddX = 0.0f, float AddY = 0.0f, int32 Octaves = 6, float Delta = 0.0f, float Scale = 2.05f);

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
