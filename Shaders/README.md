# Talus shaders

HLSL compute kernels (`.usf` files) live here, arriving in milestone 2.

At module startup, `FTalusModule` maps the virtual shader path `/Plugin/Talus/`
to this directory, so kernels can be referenced as e.g.
`/Plugin/Talus/Private/TalusNoise.usf`.

This directory must exist: `AddShaderSourceDirectoryMapping` asserts on a
missing directory, and the module guards the call — but shipping the directory
is what makes the mapping actually happen.
