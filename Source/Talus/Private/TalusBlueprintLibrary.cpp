// Copyright (c) 2026 parthnaik92. Licensed under the MIT License. See LICENSE.

#include "TalusBlueprintLibrary.h"
#include "TalusSubsystem.h"
#include "Engine/Engine.h"

UTalusSubsystem* UTalusBlueprintLibrary::GetTalusSubsystem(const UObject* WorldContextObject)
{
	UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull) : nullptr;
	return World ? World->GetSubsystem<UTalusSubsystem>() : nullptr;
}

UTalusHeightfield* UTalusBlueprintLibrary::CreateTalusHeightfield(const UObject* WorldContextObject, int32 Size)
{
	UTalusSubsystem* Subsystem = GetTalusSubsystem(WorldContextObject);
	return Subsystem ? Subsystem->CreateHeightfield(Size) : nullptr;
}
