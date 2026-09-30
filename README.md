# Talus

Runtime GPU procedural terrain heightmap generation for Unreal Engine 5.

Talus generates terrain heightfields **at runtime, on the GPU**, and hands them to a
terrain renderer as a texture. It is the *generator* half of the pipeline — meshing,
LOD, and collision are left to a dedicated terrain renderer such as
[Shader World](https://www.fab.com/listings/ee4bcbaf-36ea-4dd9-aade-068e1a8a4794).

The generation algorithms are inspired by [wgen](https://github.com/jice-nospam/wgen)
(MIT), reimplemented natively for Unreal: noise generators plus thermal and fluvial
erosion, all running as compute shaders so a full 2048² heightfield regenerates in
well under a second.

**Status: Milestone 2** — GPU noise generation is in (FBM); the heightfield
texture plumbing from M1 is unchanged. See [Milestones](#milestones).

## Milestones

1. **Plugin skeleton + texture plumbing** (current) — module, `UTalusHeightfield`
   (transient R32F texture), `UTalusSubsystem` (per-world manager + named
   registry, reachable from C++ and Blueprints), `UTalusBlueprintLibrary`
   convenience nodes, `ATalusTerrainActor` debug test actor with an analytic
   test pattern, shader directory mapping for later `.usf` kernels.
2. **Noise generators on GPU** (in progress — FBM done) — compute shaders
   (HLSL) transliterated from wgen's proven GPU kernels. FBM is implemented;
   Ridged, Plateau, Hills, MidPoint follow the same pattern.
3. **Thermal erosion on GPU** — stencil kernel, same algorithm as wgen's GPU twin.
4. **Fluvial erosion on GPU** — wgen's parallel variant (iterative depression fill +
   parallel drainage accumulation). Validated against `wgen --export` GPU-mode output.
5. **Polish** — generator stack presets, seed UI, async regen, docs.

## Building (Windows)

1. Clone this repo into your project's `Plugins` folder, so you end up with
   `<YourProject>/Plugins/Talus/Talus.uplugin`.
2. Right-click `<YourProject>.uproject` → **Generate Visual Studio project files**.
3. Open the `.sln`, build the **Development Editor / Win64** configuration
   (or just open the `.uproject` and click *Yes* when Unreal offers to build the
   missing Talus module).
4. Open the project. The Talus module loads at startup; check the Output Log for
   `LogTalus` lines.

## Using Talus (no actor required)

The test actor above is optional. The real API is `UTalusSubsystem`, which exists
automatically in every world:

**C++:**
```cpp
if (UTalusSubsystem* Talus = GetWorld()->GetSubsystem<UTalusSubsystem>())
{
    // Your own heightfield, ready to use:
    UTalusHeightfield* HF = Talus->CreateHeightfield(2048);

    // ...or the shared one several systems can look up by name:
    UTalusHeightfield* Shared = Talus->GetOrCreateSharedHeightfield(TEXT("MainTerrain"), 2048);
    UTexture2D* Tex = Shared->GetHeightmapTexture();
}
```

**Blueprints:** the **Talus** category has `Get Talus Subsystem` and
`Create Talus Heightfield` nodes (world-context aware) — call them from anywhere:
level Blueprint, game mode, UI, another plugin. The heightfield itself exposes
`GenerateFbm` (GPU noise) and `FillTestPattern` (debug cone).

## How generation works (milestone 2)

`UTalusHeightfield::GenerateFbm(...)` dispatches the `Talus.Fbm` compute kernel
via RDG. One architectural note: a transient `UTexture2D` is created SRV-only,
and D3D12 requires the UAV flag at resource creation time — so the kernel cannot
write into it directly. Instead it writes into an RDG working texture, which is
then copied into the heightfield's `UTexture2D`. The texture your renderer binds
never changes identity, so milestone 1 integrations keep working untouched.

Parameters mirror wgen's `FbmConf` (defaults are wgen's): `Seed` (1337),
`MulX`/`MulY` zoom (2.2 — higher packs smaller features across the map),
`AddX`/`AddY` sample offset (0), `Octaves` (6), `Delta` base offset (0),
`Scale` bump height (2.05). The kernel samples wgen's 512-wide virtual plane
with persistence 0.5 and lacunarity 2π/3.

One deliberate deviation from wgen: its GPU noise reproduces its CPU
permutation tables bit-exactly; Talus ships GPU-only with no CPU twin to match,
so the tables are replaced by a self-contained integer hash. Same terrain class,
much simpler port.

## Validating milestones 1–2 (Shader World handoff)

1. Drag an **ATalusTerrainActor** into your level and set **Heightmap Size**
   (1024 is a good start).
2. Play in editor — at BeginPlay the actor builds an R32F heightmap texture
   containing GPU-generated FBM noise (or press **Regenerate** in the actor's
   Details panel without playing). `FillTestPattern()` is still available as a
   Blueprint node if you want the old analytic cone for comparison.
3. In your Shader World generator material (`MF_Define_*`), sample
   `Heightfield → GetHeightmapTexture()` by world XZ and use it as the height.
   You should see FBM terrain rendered.
4. If it shows up, the Talus → texture → renderer contract works, and further
   generators (M2–M4) slot in without changing anything on the renderer side.

Notes:

- The texture holds normalized heights in **0..1**; scale to world Z in your material.
- Shader World quantizes internally to ~1 cm vertically; keep that in mind when
  choosing your world Z range.

## License

MIT — see [LICENSE](LICENSE). Do what you want with it.
