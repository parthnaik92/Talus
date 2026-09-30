// Copyright (c) 2026 parthnaik92. Licensed under the MIT License. See LICENSE.

#include "TalusHeightfield.h"
#include "Engine/Texture2D.h"
#include "TalusModule.h"

void UTalusHeightfield::Initialize(int32 Size)
{
	Size = FMath::Max(Size, 1);
	HeightmapSize = Size;

	HeightmapTexture = UTexture2D::CreateTransient(Size, Size, PF_R32_FLOAT);
	if (HeightmapTexture)
	{
		HeightmapTexture->SRGB = false;
		HeightmapTexture->UpdateResource();
		UE_LOG(LogTalus, Log, TEXT("Created %dx%d R32F heightmap texture."), Size, Size);
	}
	else
	{
		UE_LOG(LogTalus, Error, TEXT("Failed to create heightmap texture."));
	}
}

void UTalusHeightfield::FillTestPattern()
{
	if (!HeightmapTexture || HeightmapSize <= 0)
	{
		UE_LOG(LogTalus, Warning, TEXT("FillTestPattern called before Initialize; ignoring."));
		return;
	}

	const int32 Size = HeightmapSize;
	TArray<float> Data;
	Data.SetNumUninitialized(Size * Size);

	// Smooth radial cone with a gentle ripple: 1 at the center, 0 at the
	// corners. Simple, deterministic, and visibly 3D in a terrain renderer.
	const float Half = (Size - 1) * 0.5f;
	for (int32 Y = 0; Y < Size; ++Y)
	{
		for (int32 X = 0; X < Size; ++X)
		{
			const float DX = (X - Half) / Half;
			const float DY = (Y - Half) / Half;
			const float R = FMath::Sqrt(DX * DX + DY * DY);

			float H = FMath::Clamp(1.0f - R, 0.0f, 1.0f);
			H = H * H * (3.0f - 2.0f * H); // smoothstep
			H += 0.05f * FMath::Sin(R * 20.0f) * H;

			Data[Y * Size + X] = FMath::Clamp(H, 0.0f, 1.0f);
		}
	}

	FUpdateTextureRegion2D Region(0, 0, 0, 0, Size, Size);
	HeightmapTexture->UpdateTextureRegions(
		0, 1, &Region,
		Size * sizeof(float), sizeof(float),
		reinterpret_cast<uint8*>(Data.GetData()));
}
