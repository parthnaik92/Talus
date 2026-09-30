// Copyright (c) 2026 parthnaik92. Licensed under the MIT License. See LICENSE.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TalusTerrainActor.generated.h"

class UTalusHeightfield;

/**
 * Milestone 1 test actor: drop it in a level, set the heightmap size, and it
 * builds a UTalusHeightfield with an analytic test pattern at BeginPlay.
 *
 * To validate the handoff, bind Heightfield->GetHeightmapTexture() into your
 * terrain material (e.g. Shader World's generator material, sampled by world
 * XZ) and confirm the test cone renders as terrain.
 */
UCLASS()
class TALUS_API ATalusTerrainActor : public AActor
{
	GENERATED_BODY()

public:
	ATalusTerrainActor();

	/** Heightmap resolution (Size x Size). Applied on Regenerate(). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Talus", meta = (ClampMin = "1", ClampMax = "4096"))
	int32 HeightmapSize = 1024;

	/** The owned heightfield. Null until Regenerate() / BeginPlay. */
	UPROPERTY(BlueprintReadOnly, Category = "Talus")
	TObjectPtr<UTalusHeightfield> Heightfield = nullptr;

	/** (Re)builds the heightfield texture with the current test pattern. */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Talus")
	void Regenerate();

protected:
	virtual void BeginPlay() override;
};
