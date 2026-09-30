// Copyright (c) 2026 parthnaik92. Licensed under the MIT License. See LICENSE.

#include "TalusTerrainActor.h"
#include "TalusHeightfield.h"
#include "TalusSubsystem.h"

ATalusTerrainActor::ATalusTerrainActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATalusTerrainActor::BeginPlay()
{
	Super::BeginPlay();
	Regenerate();
}

void ATalusTerrainActor::Regenerate()
{
	UWorld* World = GetWorld();
	UTalusSubsystem* Subsystem = World ? World->GetSubsystem<UTalusSubsystem>() : nullptr;
	if (!Subsystem)
	{
		return;
	}

	// One shared heightfield per actor instance, reused across Regenerate()
	// calls (and recreated if the size changed). The actor is just a debug
	// helper; the subsystem owns the heightfield.
	const FName Key(*FString::Printf(TEXT("TalusTerrainActor_%s"), *GetFName().ToString()));
	Heightfield = Subsystem->GetOrCreateSharedHeightfield(Key, HeightmapSize);
}
