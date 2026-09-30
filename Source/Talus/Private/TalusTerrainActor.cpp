// Copyright (c) 2026 parthnaik92. Licensed under the MIT License. See LICENSE.

#include "TalusTerrainActor.h"
#include "TalusHeightfield.h"

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
	if (!Heightfield)
	{
		Heightfield = NewObject<UTalusHeightfield>(this);
	}

	Heightfield->Initialize(HeightmapSize);
	Heightfield->FillTestPattern();
}
