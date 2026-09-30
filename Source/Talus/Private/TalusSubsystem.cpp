// Copyright (c) 2026 parthnaik92. Licensed under the MIT License. See LICENSE.

#include "TalusSubsystem.h"
#include "TalusHeightfield.h"
#include "TalusModule.h"

UTalusHeightfield* UTalusSubsystem::CreateHeightfield(int32 Size)
{
	UTalusHeightfield* Heightfield = NewObject<UTalusHeightfield>(this);
	Heightfield->Initialize(Size);
	Heightfield->FillTestPattern(); // M1 scaffolding; replaced by GPU generation in M2.
	UE_LOG(LogTalus, Log, TEXT("Created heightfield %dx%d."), Heightfield->HeightmapSize, Heightfield->HeightmapSize);
	return Heightfield;
}

UTalusHeightfield* UTalusSubsystem::GetOrCreateSharedHeightfield(FName Name, int32 Size)
{
	if (TObjectPtr<UTalusHeightfield>* Found = SharedHeightfields.Find(Name))
	{
		if (*Found && (*Found)->HeightmapSize == Size)
		{
			return *Found;
		}
		// Size changed: drop the old one and recreate below.
		SharedHeightfields.Remove(Name);
	}

	UTalusHeightfield* Heightfield = CreateHeightfield(Size);
	SharedHeightfields.Add(Name, Heightfield);
	return Heightfield;
}

void UTalusSubsystem::Deinitialize()
{
	SharedHeightfields.Empty();
	Super::Deinitialize();
}
