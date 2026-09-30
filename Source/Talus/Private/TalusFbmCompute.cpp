// Copyright (c) 2026 parthnaik92. Licensed under the MIT License. See LICENSE.

#include "TalusHeightfield.h"
#include "Engine/Texture2D.h"
#include "GlobalShader.h"
#include "ShaderParameterStruct.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "DataDrivenShaderPlatformInfo.h"
#include "RHICommandList.h"
#include "RHIResources.h"
#include "TalusModule.h"

/** FBM noise compute kernel. HLSL: /Plugin/Talus/Private/TalusFbm.usf. */
class FTalusFbmCS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FTalusFbmCS);
	SHADER_USE_PARAMETER_STRUCT(FTalusFbmCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float>, OutHeightmap)
		SHADER_PARAMETER(FIntPoint, MapSize)
		SHADER_PARAMETER(uint32, Seed)
		SHADER_PARAMETER(float, XCoef)
		SHADER_PARAMETER(float, YCoef)
		SHADER_PARAMETER(float, AddX)
		SHADER_PARAMETER(float, AddY)
		SHADER_PARAMETER(uint32, Octaves)
		SHADER_PARAMETER(float, Delta)
		SHADER_PARAMETER(float, Scale)
	END_SHADER_PARAMETER_STRUCT()

public:
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
};

IMPLEMENT_GLOBAL_SHADER(FTalusFbmCS, "/Plugin/Talus/Private/TalusFbm.usf", "MainCS", SF_Compute);

void UTalusHeightfield::GenerateFbm(int32 Seed, float MulX, float MulY, float AddX, float AddY, int32 Octaves, float Delta, float Scale)
{
	if (!HeightmapTexture || HeightmapSize <= 0)
	{
		UE_LOG(LogTalus, Warning, TEXT("GenerateFbm called before Initialize; ignoring."));
		return;
	}
	const int32 Size = HeightmapSize;

	ENQUEUE_RENDER_COMMAND(TalusGenerateFbm)(
		[HF = TStrongObjectPtr<UTalusHeightfield>(this), Size, Seed, MulX, MulY, AddX, AddY, Octaves, Delta, Scale]
		(FRHICommandListImmediate& RHICmdList)
		{
			// The destination UTexture2D is created SRV-only (transient
			// textures get no UAV flag), and D3D12 requires the UAV flag at
			// resource creation time -- so the kernel cannot write into it
			// directly. Generate into an RDG working texture, then copy into
			// the UTexture2D, which stays the Shader World-facing asset.
			UTexture2D* DestTexture = HF.IsValid() ? HF->GetHeightmapTexture() : nullptr;
			FTextureResource* DestRes = DestTexture ? DestTexture->GetResource() : nullptr;
			FTextureRHIRef DestRHI = DestRes ? DestRes->GetTextureRHI() : nullptr;
			if (!DestRHI.IsValid())
			{
				UE_LOG(LogTalus, Error, TEXT("GenerateFbm: destination texture has no RHI resource."));
				return;
			}

			FRDGBuilder GraphBuilder(RHICmdList);

			const FRDGTextureDesc WorkDesc = FRDGTextureDesc::Create2D(
				FIntPoint(Size, Size),
				PF_R32_FLOAT,
				FClearValueBinding::Black,
				ETextureCreateFlags::ShaderResource | ETextureCreateFlags::UAV);
			FRDGTextureRef WorkTex = GraphBuilder.CreateTexture(WorkDesc, TEXT("Talus.WorkingHeight"));

			TShaderMapRef<FTalusFbmCS> Shader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
			FTalusFbmCS::FParameters* Params = GraphBuilder.AllocParameters<FTalusFbmCS::FParameters>();
			Params->OutHeightmap = WorkTex;
			Params->MapSize = FIntPoint(Size, Size);
			Params->Seed = (uint32)Seed;
			Params->XCoef = MulX / 400.0f; // wgen's zoom convention
			Params->YCoef = MulY / 400.0f;
			Params->AddX = AddX;
			Params->AddY = AddY;
			Params->Octaves = (uint32)FMath::Max(Octaves, 1);
			Params->Delta = Delta;
			Params->Scale = Scale;

			FComputeShaderUtils::AddPass(
				GraphBuilder,
				RDG_EVENT_NAME("Talus.Fbm"),
				Shader,
				Params,
				FComputeShaderUtils::GetGroupCount(FIntPoint(Size, Size), FIntPoint(8, 8)));

			// Retain the working texture's RHI resource past graph execution
			// for the copy below. FRDGTextureRef itself must never escape.
			FTextureRHIRef WorkRHI;
			GraphBuilder.QueueTextureExtraction(WorkTex, &WorkRHI);
			GraphBuilder.Execute();

			if (!WorkRHI.IsValid())
			{
				UE_LOG(LogTalus, Error, TEXT("GenerateFbm: failed to extract working texture."));
				return;
			}

			// Working texture -> UTexture2D. Copy destinations need no UAV flag.
			// From-state Unknown forces a full transition regardless of what
			// RDG left behind. Fire-and-forget: no flush, the texture updates
			// a frame or two later.
			RHICmdList.Transition(FRHITransitionInfo(WorkRHI, ERHIAccess::Unknown, ERHIAccess::CopySrc));
			RHICmdList.Transition(FRHITransitionInfo(DestRHI, ERHIAccess::Unknown, ERHIAccess::CopyDest));
			RHICmdList.CopyTexture(WorkRHI, DestRHI, FRHICopyTextureInfo());
			RHICmdList.Transition(FRHITransitionInfo(DestRHI, ERHIAccess::CopyDest, ERHIAccess::SRVMask));
		});
}
