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

**Status: Milestone 1** — plugin skeleton plus the heightfield texture plumbing.
No real generators yet; see [Milestones](#milestones).

## Milestones

1. **Plugin skeleton + texture plumbing** (current) — module, `UTalusHeightfield`
   (transient R32F texture), `ATalusTerrainActor` test actor with an analytic test
   pattern, shader directory mapping for later `.usf` kernels.
2. **Noise generators on GPU** — Hills, FBM, Ridged, MidPoint, Plateau as compute
   shaders (HLSL), transliterated from wgen's proven GPU kernels.
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

## Validating milestone 1 (Shader World handoff)

1. Drag an **ATalusTerrainActor** into your level and set **Heightmap Size**
   (1024 is a good start).
2. Play in editor — at BeginPlay the actor builds an R32F heightmap texture
   containing a smooth radial test cone (or press **Regenerate** in the actor's
   Details panel without playing).
3. In your Shader World generator material (`MF_Define_*`), sample
   `Heightfield → GetHeightmapTexture()` by world XZ and use it as the height.
   You should see the test cone rendered as terrain.
4. If the cone shows up, the Talus → texture → renderer contract works, and
   milestone 2 can swap the test pattern for real generators without changing
   anything on the renderer side.

Notes:

- The texture holds normalized heights in **0..1**; scale to world Z in your material.
- Shader World quantizes internally to ~1 cm vertically; keep that in mind when
  choosing your world Z range.

## License

MIT — see [LICENSE](LICENSE). Do what you want with it.
