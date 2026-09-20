# SMOL-V: like Vulkan/Khronos SPIR-V, but smaller. ![Build Status](https://github.com/aras-p/smol-v/workflows/build_and_test/badge.svg)

TL;DR: makes your SPIR-V shaders about 3x smaller, and decodes very fast. If you like to compress them with regular compressor like zlib/lz4/zstd,
then smol-v before the compression can still make them 2x smaller.

![](/size_chart.png)

## Overview

SMOL-V encodes Vulkan/Khronos [SPIR-V](https://www.khronos.org/registry/spir-v/)
format programs into a form that is *smoller*, and is more
compressible. Normally no changes to the programs are done; they decode
into exactly same program as what was encoded. Optionally, debug information
can be removed too.

SPIR-V is a very verbose format, several times larger than same programs expressed in other
shader formats *(e.g. DX11 bytecode, GLSL, DX9 bytecode etc.)*. The SSA-form with ever increasing
IDs is not very appreciated by regular data compressors either. SMOL-V does several things
to improve this:

- Many words, especially ones that most often have small values, are encoded using
  ["varint" scheme](https://developers.google.com/protocol-buffers/docs/encoding) (1-5 bytes per
  word, with just one byte for values in 0..127 range).
- Some IDs used in the program are delta-encoded, relative to previously seen IDs (e.g. Result
  IDs). Often instructions reference things that were computed just before, so this results in
  small deltas. These values are also encoded using "varint" scheme.
- Reordering instruction opcodes so that the most common ones are the smallest values, for smaller
  varint encoding.
- Encoding several instructions in a more compact form, e.g. the "typical <=4 component swizzle"
  shape of a VectorShuffle instruction, or sequences of MemberDecorate instructions.

A somewhat similar utility is [spirv-remap from glslang](https://github.com/KhronosGroup/glslang/blob/main/README-spirv-remap.txt).

See [this blog post](https://aras-p.info/blog/2016/09/01/SPIR-V-Compression/) for more information about
how I did SMOL-V.


## Usage

Add [`source/smolv.h`](source/smolv.h) and [`source/smolv.cpp`](source/smolv.cpp) to your C++ project build.
It might require C++11 or somesuch. There are 
[Github Actions](https://github.com/aras-p/smol-v/actions) set up for this project, that build on Windows (VS2022),
macOS (Xcode 15) and Linux (Ubuntu 22 / gcc 11).

`smolv::Encode` and `smolv::Decode` is the basic functionality. See [smolv.h](source/smolv.h).

SPIR-V versions up to and including 1.6 are supported currently.

Other functions are for development/statistics purposes, to figure out frequencies and
distributions of the instructions.

There's a test + compression benchmarking suite in `testing/testmain.cpp`, using that needs adding
other files under testing/external to the build too (3rd party code: glslang remapper 14.3.0, Zstd 1.5.7, LZ4 1.10, miniz).

## Changelog

See [**Changelog**](Changelog.md).


## Limitations / TODO

- SPIR-V where the words got stored in big-endian layout is not supported yet.
- The whole thing might not work on Big-Endian CPUs. It might, but I'm not 100% sure.
- Not much prevention is done against malformed/corrupted inputs, TODO.
- Out of memory cases are not handled. The code will either throw exception
  or crash, depending on your compilation flags.


## License

Code itself: **Public Domain** or **MIT**, pick whichever works better for you.

There is 3rd party code under the testing framework (`testing/external`); it is not required for
using SMOL-V. Most of that code ([glslang](https://github.com/KhronosGroup/glslang),
[LZ4](https://github.com/Cyan4973/lz4), [Zstd](https://github.com/facebook/zstd), [sokol_time.h](https://github.com/floooh/sokol/blob/master/sokol_time.h))
is BSD or zlib-licensed, and taken from github repositories of the respective projects. [miniz](https://github.com/richgel999/miniz)
is public domain.

There are SPIR-V binary shader dumps under `tests/spirv-dumps` for compression testing;
these are not required for using SMOL-V. Not sure how to appropriately
"license" them (but hey they are kinda useless by themselves out of context),
so I'll go with this: "Binary shader dumps under 'tests' folder are only to be
used for SMOL-V testing". Details on them:

* `tests/spirv-dumps/dota2` - some shaders from [DOTA2](http://blog.dota2.com/), Copyright Valve Corporation, all rights reserved.
* `tests/spirv-dumps/shadertoy` - several most popular shaders from [Shadertoy](https://www.shadertoy.com/), converted to Vulkan
  SPIR-V via glslang. Copyrights by their individual authors (filename matches last component of shadertoy URL).
* `tests/spirv-dumps/talos` - some shaders from [The Talos Principle](http://www.croteam.com/talosprinciple/),
  Copyright (c) 2002-2016 Croteam All rights reserved.
* `tests/spirv-dumps/unity` - various [Unity](https://unity3d.com/) shaders, produced
  through a HLSL -> DX11 bytecode -> HLSLcc -> glslang toolchain.
* `tests/spirv-dumps/blender` - various [Blender](https://www.blender.org/) shaders.
* `tests/spirv-dumps/godot` - various [Godot](https://godotengine.org/) shaders.



## Results

As of 2026 Sep 20, results on 427 shaders (under `tests/spirv-dumps`) are:

```
Compressed with <none>:
Raw       11000.3KB 100.0%
Remapper  10536.3KB  95.8%
SmolV      3586.9KB  32.6%
Compressed with zlib:
Raw        3062.7KB  27.8%
Remapper   2943.0KB  26.8%
SmolV      1419.1KB  12.9%
Compressed with LZ4 HC:
Raw        3741.6KB  34.0%
Remapper   3479.6KB  31.6%
SmolV      1564.5KB  14.2%
Compressed with Zstandard:
Raw        2587.8KB  23.5%
Remapper   2153.8KB  19.6%
SmolV      1179.1KB  10.7%
Compressed with Zstandard 20:
Raw        1937.8KB  17.6%
Remapper   1609.7KB  14.6%
SmolV      1026.8KB   9.3%
```

Decoding these 427 shaders from SMOL-V back into SPIR-V takes 18.7ms (VS2022, x64 Release, AMD Ryzen 5950X, one thread).

* "Raw" is just raw SPIR-V, with no extra processing.
* "Remapper" is spirv-remap from glslang, with debug info stripping.
* SmolV is what you're looking at, with debug info stripping too.
* zlib, LZ4HC and Zstd are general compression algorithms at default settings (Zstd20 is Zstd compression with almost max setting of 20).
* Compression is done on the whole blob of all the test programs (not individually for each program).
